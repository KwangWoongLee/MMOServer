#pragma once

class GameSession : public PacketSession
{
public:
	GameSession();

	~GameSession()
	{
		cout << "~GameSession" << endl;
	}

	virtual void OnConnected() override;
	virtual void OnRecvPacket(PacketHeader header, google::protobuf::io::CodedInputStream& payloadInputStream) override;
	virtual void OnSend(uint32_t numOfBytes) override;
	virtual void OnDisconnected() override;

	UserRef GetUser() { return mUser; }
	void SetUser(UserRef user) { mUser = user; }

	bool Test = false;

private:
	UserRef mUser = nullptr;
};