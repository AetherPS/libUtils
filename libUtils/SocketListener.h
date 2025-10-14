#pragma once

class SocketListener
{
private:
	void(*ClientCallBack)(void* tdParam, SceNetId Sock, SceNetInAddr sin_addr);
	bool ServerRunning;
	void* ThreadParameter;
	SceNetId ServerSocket;
	bool Verbose;

	void ListenThread(int protocol, unsigned short port, int timeOutSeconds);

public:
	SocketListener(
		void(*ClientCallBack)(void* tdParam, SceNetId Sock, SceNetInAddr sin_addr),
		void(*ConnectionLost)(void* tdParam),
		void(*ConnectionRestored)(void* tdParam),
		void* tdParam,
		unsigned short Port,
		int timeOutSeconds = 2,
		int protocol = SCE_NET_IPPROTO_TCP, 
		bool verbose = false);
	~SocketListener();
};