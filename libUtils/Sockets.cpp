#include "stdafx.h"
#include <net.h>
#include "Sockets.h"

SceNetId Sockets::ConnectLocal(char* address, int timeOutSeconds)
{
	// Set up socket params.
	SceNetSockaddrUn addr = { 0 };
	addr.sun_family = SCE_NET_AF_LOCAL;
	strncpy(addr.sun_path, address, sizeof(addr.sun_path));

	// Create socket.
	auto sock = sceNetSocket("", SCE_NET_AF_LOCAL, SCE_NET_SOCK_STREAM, 0);
	if (sock < 0)
	{
		Logger::Error("%s Failed to allocate sock: %llX %llX\n", __FUNCTION__, sock, *sceNetErrnoLoc());
		return -1;
	}

	// Set connection/transmission time out.
	int sock_timeout = timeOutSeconds * 1000000;
	sceNetSetsockopt(sock, SCE_NET_SOL_SOCKET, SCE_NET_SO_CONNECTTIMEO, &sock_timeout, sizeof(sock_timeout));
	sceNetSetsockopt(sock, SCE_NET_SOL_SOCKET, SCE_NET_SO_SNDTIMEO, &sock_timeout, sizeof(sock_timeout));
	sceNetSetsockopt(sock, SCE_NET_SOL_SOCKET, SCE_NET_SO_RCVTIMEO, &sock_timeout, sizeof(sock_timeout));

	// Connect to the socket.
	auto res = sceNetConnect(sock, (SceNetSockaddr*)&addr, sizeof(addr));
	if (!res)
		return sock;
	else
	{
		Logger::Error("%s sceNetConnect(): %llX %llX\n", __FUNCTION__, res, *sceNetErrnoLoc());
		return -1;
	}
}

SceNetId Sockets::Connect(SceNetInAddr_t address, uint16_t port, int timeOutSeconds)
{
	// Set up socket params.
	SceNetSockaddrIn addr = { 0 };
	addr.sin_family = SCE_NET_AF_INET;
	addr.sin_addr.s_addr = address;
	addr.sin_port = sceNetHtons(port);

	// Create socket.
	auto sock = sceNetSocket("", SCE_NET_AF_INET, SCE_NET_SOCK_STREAM, SCE_NET_IPPROTO_TCP);
	if (sock < 0)
	{
		Logger::Error("%s Failed to allocate sock: %llX %llX\n", __FUNCTION__, sock, *sceNetErrnoLoc());
		return -1;
	}

	// Set connection/transmission time out.
	int sock_timeout = timeOutSeconds * 1000000;
	sceNetSetsockopt(sock, SCE_NET_SOL_SOCKET, SCE_NET_SO_CONNECTTIMEO, &sock_timeout, sizeof(sock_timeout));
	sceNetSetsockopt(sock, SCE_NET_SOL_SOCKET, SCE_NET_SO_SNDTIMEO, &sock_timeout, sizeof(sock_timeout));
	sceNetSetsockopt(sock, SCE_NET_SOL_SOCKET, SCE_NET_SO_RCVTIMEO, &sock_timeout, sizeof(sock_timeout));

	// Connect to the socket.
	auto res = sceNetConnect(sock, (SceNetSockaddr*)&addr, sizeof(addr));
	if (!res)
		return sock;
	else
	{
		Logger::Error("%s sceNetConnect(): %llX %llX\n", __FUNCTION__, res, *sceNetErrnoLoc());
		return -1;
	}
}

SceNetId Sockets::Connect(const char* address, uint16_t port, int timeOutSeconds)
{
	SceNetInAddr_t s_addr;
	sceNetInetPton(SCE_NET_AF_INET, address, &s_addr);

	return Connect(s_addr, port, timeOutSeconds);
}

bool Sockets::SendInt(SceNetId Sock, int val)
{
	auto res = sceNetSend(Sock, &val, sizeof(int), 0);
	if (res <= 0)
	{
		Logger::Error("%s: Failed to send %llX\n", __FUNCTION__, res);

		return false;
	}

	return true;
}

bool Sockets::RecvInt(SceNetId Sock, int* val)
{
	auto res = sceNetRecv(Sock, val, sizeof(int), 0);
	if (res <= 0)
	{
		Logger::Error("%s: Failed to recv %llX\n", __FUNCTION__, res);

		return false;
	}

	return true;
}

bool Sockets::SendLargeData(SceNetId Sock, unsigned char* data, size_t dataLen)
{
	unsigned char* CurrentPosition = data;
	size_t DataLeft = dataLen;
	int res = 0;

	while (DataLeft > 0)
	{
		size_t DataChunkSize = std::min((size_t)8192, DataLeft);

		res = sceNetSend(Sock, CurrentPosition, DataChunkSize, 0);

		if (res < 0)
		{
			return false;
		}

		DataLeft -= res;
		CurrentPosition += res;
	}

	return true;
}

bool Sockets::RecvLargeData(SceNetId Sock, unsigned char* data, size_t dataLen)
{
	size_t DataLeft = dataLen;
	int Received = 0;
	int res = 0;

	while (DataLeft > 0)
	{
		size_t DataChunkSize = std::min((size_t)8192, DataLeft);
		res = sceNetRecv(Sock, data + Received, DataChunkSize, 0);

		if (res < 0)
		{
			return false;
		}

		Received += res;
		DataLeft -= res;
	}

	return true;
}

bool Sockets::SendWithSize(SceNetId sock, unsigned char* data, int len)
{
	// Send the size of our packet.
	if (!SendInt(sock, len))
		return false;

	// Send the packet data.
	if (!SendLargeData(sock, data, len))
		return false;

	return true;
}

std::vector<uint8_t> Sockets::ReceiveWithSize(SceNetId sock)
{
	// Recieve the size of the incoming packet.
	int len = 0;
	if (!RecvInt(sock, &len))
		return std::vector<uint8_t>();

	// Create temporary buffer for data.
	auto data = std::vector<uint8_t>(len);

	// Recieve the incoming packet.
	if (!RecvLargeData(sock, data.data(), len))
		return std::vector<uint8_t>();

	return data;
}