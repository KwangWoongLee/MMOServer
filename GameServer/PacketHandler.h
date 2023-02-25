#pragma once

extern std::vector<function<void(PacketSessionRef, PacketHeader, google::protobuf::io::CodedInputStream&)>> HandleFuncs;

class PacketHandler
{
public:
	static void Init();
	static void C_PONG(PacketSessionRef session, PacketHeader header, google::protobuf::io::CodedInputStream& inputStream);
	static void C_ENTER_GAME(PacketSessionRef session, PacketHeader header, google::protobuf::io::CodedInputStream& inputStream);
	static void C_ACTION(PacketSessionRef session, PacketHeader header, google::protobuf::io::CodedInputStream& inputStream);
} ;

