#pragma once
#include "stdafx.h"


#include "CircularBuffer.h"
#include "SocketUtil.h"
#include "IOCP.h"
#include "IOEvent.h"

namespace
{
	enum class EIOCPSessionState : uint8_t
	{
		NONE,
		CONNECTING,
		CONNECTED,
		DISCONNECTING,
		DISCONNECTED
	};

	enum class EDisconnectReason
	{
		NONE,
		EXPLICIT_CALL,
		RECV_ZERO,
		SEND_ZERO,
		RECV_OVERFLOW,
		HANDLE_ERROR,
		INVALID_STATE,
		SEND_BUFFER_OVERFLOW,
		INVALID_OPERATION,
	};

	const char* ToString(EDisconnectReason const reason)
	{
		switch (reason)
		{
		case EDisconnectReason::EXPLICIT_CALL:
			{
				return "Explicit Disconnect";
			}
		case EDisconnectReason::RECV_ZERO:
			{
				return "Client Closed Connection (Recv 0)";
			}
		case EDisconnectReason::SEND_ZERO:
			{
				return "Send Completed with 0 Bytes";
			}
		case EDisconnectReason::RECV_OVERFLOW:
			{
				return "Recv Buffer Overwrite Attempted";
			}
		case EDisconnectReason::HANDLE_ERROR:
			{
				return "Socket Handle Error";
			}
		case EDisconnectReason::INVALID_STATE:
			{
				return "Invalid State for Operation";
			}
		default:
			{
				return "Unknown Reason";
			}
		}
	}
}

class IOCPSession final
    : public IIOCPObject
{
public:
	void Dispatch(Overlapped const* iocpEvent, uint32_t const numOfBytes = 0) override;

	bool SetSockAddr();

	bool Connect();
	void Disconnect(EDisconnectReason const reason);
	void Send(const char* buffer, uint32_t contentSize);


	void OnAcceptCompleted();
	void OnConnectCompleted();
	void OnDisconnectCompleted();
	void OnRecvCompleted(uint32_t const transferred);
	void OnSendCompleted(uint32_t const transferred);

private:
	void setConnected();
	void setDisconnected();

	bool asyncConnect();
	void asyncDisconnect();
	void asyncRecv();
	void asyncSend();

	void handleError(int32_t const errorCode)
	{
		switch (errorCode)
		{
		case WSAECONNRESET:
		case WSAECONNABORTED:
			Disconnect("Handle Error");
			break;
		default:
			cout << "Handle Error : " << errorCode << endl;
			break;
		}
	}

private:
	char _acceptBuf[64] = {};
	SocketAddress _sockAddress;

	std::atomic<EIOCPSessionState> _state{ EIOCPSessionState::NONE };

	CircularBuffer _recvBuffer{ 0x10000 }; // config·Î »¬ Áö

	std::mutex _sendMutex;
	CircularBuffer _sendBuffer{ 0x10000 }; // config·Î »¬ Áö
	bool _isSendPending{};
};