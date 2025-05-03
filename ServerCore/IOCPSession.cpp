#include "stdafx.h"
#include "IOCPSession.h"

char const* ToString(EDisconnectReason const reason)
{
    switch (reason)
    {
    case EDisconnectReason::EXPLICIT_CALL: return "Explicit Disconnect";
    case EDisconnectReason::RECV_ZERO: return "Client Closed Connection (Recv 0)";
    case EDisconnectReason::SEND_ZERO: return "Send Completed with 0 Bytes";
    case EDisconnectReason::RECV_OVERFLOW: return "Recv Buffer Overwrite Attempted";
    case EDisconnectReason::HANDLE_ERROR: return "Socket Handle Error";
    case EDisconnectReason::INVALID_STATE: return "Invalid State for Operation";
    default: return "Unknown Reason";
    }
}

void IOCPSession::Dispatch(Overlapped const* iocpEvent, uint32_t const numOfBytes)
{
    switch (iocpEvent->GetIOType())
    {
    case EIOType::CONNECT:
        {
    		OnConnectCompleted();
        } break;
    case EIOType::DISCONNECT:
        {
            OnDisconnectCompleted();
        } break;
    case EIOType::SEND:
        {
            OnSendCompleted(numOfBytes);
        } break;
    case EIOType::RECV:
        {
            OnRecvCompleted(numOfBytes);
        } break;
    default:
        {} break;
    }
}

bool IOCPSession::SetSockAddr()
{
    SocketAddress sockAddress;
    auto const name = reinterpret_cast<SOCKADDR*>(&sockAddress.GetSockAddr());
    int nameSize = sizeof(SOCKADDR);

    if (not ::getpeername(reinterpret_cast<SOCKET>(GetHandle()), name, &nameSize))
    {
        return false;
    }

    _sockAddress = sockAddress;
    return true;
}

bool IOCPSession::Connect()
{
    return AsyncConnect();
}

void IOCPSession::Disconnect(EDisconnectReason const reason)
{
    if (_state != EIOCPSessionState::CONNECTED)
    {
        return;
    }

    AsyncDisconnect();
}

void IOCPSession::Send(const char* buffer, uint32_t const contentSize)
{
    if (_state != EIOCPSessionState::CONNECTED)
    {
        return;
    }

    bool registerSend = false;

    {
        std::scoped_lock lock(_sendMutex);

        if (_sendBuffer.GetFreeSpaceSize() < contentSize)
        {
            return;
        }

        memcpy(_sendBuffer.GetBuffer(), buffer, contentSize);
        _sendBuffer.Commit(contentSize);

        if (!_isSendPending)
        {
            _isSendPending = true;
            registerSend = true;
        }
    }

    if (registerSend)
    {
        AsyncSend();
    }
}

void IOCPSession::SendPacket(uint16_t const packetId, google::protobuf::MessageLite& packet)
{
    if (_state != EIOCPSessionState::CONNECTED)
    {
        return;
    }

    uint8_t tempBuffer[0x10000];
    _streamWriter.Init(tempBuffer, sizeof(tempBuffer));

    if (_streamWriter.WritePacket(packetId, packet))
    {
        _streamWriter.Finalize();
        Send(reinterpret_cast<const char*>(tempBuffer), _streamWriter.GetSize());
    }
}

void IOCPSession::OnAcceptCompleted()
{
    auto const socket = reinterpret_cast<SOCKET>(GetHandle());

    std::ignore = SocketUtil::Singleton::Instance().SetKeepAlive(socket, true);
    std::ignore = SocketUtil::Singleton::Instance().SetTcpNoDelay(socket, true);

    SetSockAddr();
    SetConnected();
}

void IOCPSession::OnConnectCompleted()
{
    auto const socket = reinterpret_cast<SOCKET>(GetHandle());

    std::ignore = SocketUtil::Singleton::Instance().SetKeepAlive(socket, true);
    std::ignore = SocketUtil::Singleton::Instance().SetTcpNoDelay(socket, true);

    SetSockAddr();
    SetConnected();
}

void IOCPSession::OnDisconnectCompleted()
{
    SetDisconnected();
}

void IOCPSession::OnRecvCompleted(uint32_t const transferred)
{
    if (transferred == 0)
    {
        Disconnect(EDisconnectReason::RECV_ZERO);
        return;
    }

    if (_recvBuffer.GetFreeSpaceSize() < transferred)
    {
        Disconnect(EDisconnectReason::RECV_OVERFLOW);
        return;
    }

    _recvBuffer.Commit(transferred);

    while (true)
    {
        if (_recvBuffer.GetContiguousBytes() < sizeof(StreamHeader))
        {
            break;
        }

        auto* streamHeader = reinterpret_cast<StreamHeader*>(_recvBuffer.GetBufferStart());

        if (streamHeader->size == 0 || streamHeader->size > _recvBuffer.GetCapacity())
        {
            Disconnect(EDisconnectReason::INVALID_OPERATION);
            return;
        }

        if (_recvBuffer.GetContiguousBytes() < sizeof(StreamHeader) + streamHeader->size)
        {
            break;
        }

        _streamReader.Init(reinterpret_cast<uint8_t*>(_recvBuffer.GetBufferStart() + sizeof(StreamHeader)), streamHeader->size);

        PacketHeader packetHeader;
        const void* payload = nullptr;
        while (_streamReader.ReadPacket(packetHeader, payload))
        {
            OnRecvPacket(packetHeader.id, payload, packetHeader.size);
        }

        _recvBuffer.Remove(sizeof(StreamHeader) + streamHeader->size);
    }

    AsyncRecv();
}

void IOCPSession::OnSendCompleted(uint32_t const transferred)
{
    if (transferred == 0)
    {
        Disconnect(EDisconnectReason::SEND_ZERO);
        return;
    }

    std::scoped_lock lock(_sendMutex);

    _sendBuffer.Remove(transferred);
    _isSendPending = false;

    if (_sendBuffer.GetContiguousBytes() > 0)
    {
        _isSendPending = true;
        AsyncSend();
    }
}

void IOCPSession::SetConnected()
{
    _state = EIOCPSessionState::CONNECTED;
    OnConnected();
    AsyncRecv();
}

void IOCPSession::SetDisconnected()
{
    if (_state == EIOCPSessionState::DISCONNECTED)
    {
        return;
    }

    _state = EIOCPSessionState::DISCONNECTED;
    OnDisconnected();
}

bool IOCPSession::AsyncConnect()
{
    if (_state != EIOCPSessionState::CONNECTING)
    {
        return false;
    }

    auto const socket = reinterpret_cast<SOCKET>(GetHandle());

    if (not SocketUtil::Singleton::Instance().Bind(socket))
    {
        return false;
    }

    if (not SocketUtil::Singleton::Instance().SetReuseAddress(socket, true))
    {
        return false;
    }

    auto const connectIoEvent = Overlapped::GetObjectPoolIOEvent(EIOType::CONNECT, shared_from_this());

    DWORD numOfBytes = 0;
    if (not FnConnectEx(socket, reinterpret_cast<const SOCKADDR*>(&serverSockAddress.GetSockAddr()), sizeof(SOCKADDR_IN), nullptr, 0, &numOfBytes, connectIoEvent))
    {
        if (const auto error = WSAGetLastError(); error != WSA_IO_PENDING)
        {
            ObjectPool<Overlapped>::Singleton::Instance().Release(connectIoEvent);
            return false;
        }
    }
    return true;
}

void IOCPSession::AsyncDisconnect()
{
    auto const disconnectIoEvent = Overlapped::GetObjectPoolIOEvent(EIOType::DISCONNECT, shared_from_this());

    if (not FnDisconnectEx(reinterpret_cast<SOCKET>(GetHandle()), disconnectIoEvent, TF_REUSE_SOCKET, 0))
    {
        if (const auto error = WSAGetLastError(); error != WSA_IO_PENDING)
        {
            ObjectPool<Overlapped>::Singleton::Instance().Release(disconnectIoEvent);
            ::closesocket(reinterpret_cast<SOCKET>(GetHandle()));
        }
    }
}

void IOCPSession::AsyncRecv()
{
    if (_state != EIOCPSessionState::CONNECTED)
    {
        return;
    }

    auto const recvIoEvent = Overlapped::GetObjectPoolIOEvent(EIOType::RECV, shared_from_this());

    uint32_t freeSpace = _recvBuffer.GetFreeSpaceSize();
    if (freeSpace == 0)
    {
        //TODO: log - No free space in RecvBuffer
        return;
    }

    WSABUF wsaBuf;
    wsaBuf.buf = reinterpret_cast<char*>(_recvBuffer.GetBuffer());
    wsaBuf.len = static_cast<ULONG>(freeSpace);

    DWORD flags = 0;
    DWORD recvBytes = 0;

    if (SOCKET_ERROR == WSARecv(reinterpret_cast<SOCKET>(GetHandle()), &wsaBuf, 1, &recvBytes, &flags, recvIoEvent, NULL))
    {
        if (auto const err = WSAGetLastError(); err != WSA_IO_PENDING)
        {
            ObjectPool<Overlapped>::Singleton::Instance().Release(recvIoEvent);
            HandleError(err);
        }
    }
}

void IOCPSession::AsyncSend()
{
    if (_state != EIOCPSessionState::CONNECTED)
    {
        return;
    }

    std::scoped_lock lock(_sendMutex);

    if (_sendBuffer.GetContiguousBytes() == 0)
    {
        return;
    }

    auto const sendIoEvent = Overlapped::GetObjectPoolIOEvent(EIOType::SEND, shared_from_this());

    WSABUF wsaBuf;
    wsaBuf.buf = reinterpret_cast<CHAR*>(_sendBuffer.GetBufferStart());
    wsaBuf.len = static_cast<ULONG>(_sendBuffer.GetContiguousBytes());

    DWORD sendBytes = 0;
    DWORD flags = 0;

    if (SOCKET_ERROR == WSASend(reinterpret_cast<SOCKET>(GetHandle()), &wsaBuf, 1, &sendBytes, flags, sendIoEvent, NULL))
    {
        if (auto const err = WSAGetLastError(); err != WSA_IO_PENDING)
        {
            ObjectPool<Overlapped>::Singleton::Instance().Release(sendIoEvent);
            HandleError(err);
        }
    }
}

void IOCPSession::HandleError(int32_t const errorCode)
{
    switch (errorCode)
    {
    case WSAECONNRESET:
    case WSAECONNABORTED:
    case WSAENETRESET:
    case WSAETIMEDOUT:
        {
            Disconnect(EDisconnectReason::HANDLE_ERROR);
            break;
        }
    default:
        {
            //TODO: log - HandleError 발생, errorCode 출력
            break;
        }
    }
}