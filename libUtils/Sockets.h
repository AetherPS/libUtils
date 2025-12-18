#pragma once

typedef struct SceNetSockaddrUn
{
	uint8_t sun_len;
	SceNetSaFamily_t sun_family;
	char sun_path[104];
} SceNetSockaddrUn;

#define SCE_NET_AF_LOCAL 1

class Sockets
{
public:
	static SceNetId ConnectLocal(char* address, int timeOutSeconds);
	static SceNetId Connect(SceNetInAddr_t address, uint16_t port, int timeOutSeconds);
	static SceNetId Connect(const char* address, uint16_t port, int timeOutSeconds);
	static bool SendByte(SceNetId sock, uint8_t val);
	static bool RecvByte(SceNetId sock, uint8_t* val);
	static bool SendUInt32(SceNetId sock, uint32_t val);
	static bool RecvUInt32(SceNetId sock, uint32_t* val);
	static bool SendInt(SceNetId Sock, int val);
	static bool RecvInt(SceNetId Sock, int* val);
	static bool SendBytes(SceNetId Sock, unsigned char* data, size_t dataLen);
	static bool RecvBytes(SceNetId Sock, unsigned char* data, size_t dataLen);

	template<class T> static std::shared_ptr<T> RecieveType(SceNetId sock)
	{
		auto packet = std::make_shared<T>();
		return RecvBytes(sock, (unsigned char*)packet.get(), sizeof(T)) ? packet : nullptr;
	}

private:

};