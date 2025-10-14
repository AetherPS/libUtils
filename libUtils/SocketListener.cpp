#include "stdafx.h"
#include "SocketListener.h"

void SocketListener::ListenThread(int protocol, unsigned short port, int timeOutSeconds)
{
	SceNetSockaddrIn addr = { 0 };
	addr.sin_family = SCE_NET_AF_INET;
	addr.sin_addr.s_addr = SCE_NET_INADDR_ANY; // Any incoming address
	addr.sin_port = sceNetHtons(port); // Our desired listen port

	// Make new socket.
	ServerSocket = sceNetSocket("Listener Socket", SCE_NET_AF_INET, SCE_NET_SOCK_STREAM, protocol);

	// Set time out if defined.
	if (timeOutSeconds != 0)
	{
		int sock_timeout = 1000000 * timeOutSeconds;
		sceNetSetsockopt(ServerSocket, SCE_NET_SOL_SOCKET, SCE_NET_SO_SNDTIMEO, &sock_timeout, sizeof(sock_timeout));
		sceNetSetsockopt(ServerSocket, SCE_NET_SOL_SOCKET, SCE_NET_SO_RCVTIMEO, &sock_timeout, sizeof(sock_timeout));
	}

	// Make sure every time we can rebind to the port.
	int reusePort = 1;
	sceNetSetsockopt(ServerSocket, SCE_NET_SOL_SOCKET, SCE_NET_SO_REUSEPORT, &reusePort, sizeof(reusePort));

	auto bindError = sceNetBind(ServerSocket, (SceNetSockaddr*)&addr, sizeof(addr));
	if (bindError != 0)
	{
		Logger::Error("Failed to bind Listener to port %i\nErrorno: %X", port, bindError);

		goto Cleanup;
	}

	if (sceNetListen(ServerSocket, 100) != 0)
	{
		Logger::Error("Failed to start listening on Socket.\n");

		goto Cleanup;
	}

	while (ServerRunning)
	{
		fd_set set;
		struct timeval timeout;
		FD_ZERO(&set); /* clear the set */
		FD_SET(ServerSocket, &set); /* add our file descriptor to the set */

		timeout.tv_sec = 2;
		timeout.tv_usec = 0;

		// Wait for incoming connections.
		auto rv = select((int)ServerSocket + 1, &set, NULL, NULL, &timeout);
		if (rv == -1)
		{
			goto Cleanup;
		}
		else if (rv == 0)
		{
			if (!ServerRunning)
				goto Cleanup;

			continue;
		}
		else
		{
			if (!ServerRunning)
				goto Cleanup;

			SceNetSockaddrIn ClientAddr = { 0 };
			SceNetSocklen_t addrlen = sizeof(SceNetSockaddrIn);
			auto ClientSocket = sceNetAccept(ServerSocket, (SceNetSockaddr*)&ClientAddr, &addrlen);

			if (ClientSocket < 0 || sce_net_errno != 0)
			{
				if (Verbose)
					Logger::Error("Sock: %d, %d\n", ClientSocket, sce_net_errno);
				
				continue;
			}

			// Logger::Info("New Connection from %i.%i.%i.%i!\n", ClientAddr.sin_addr.s_addr & 0xFF, (ClientAddr.sin_addr.s_addr >> 8) & 0xFF, (ClientAddr.sin_addr.s_addr >> 16) & 0xFF, (ClientAddr.sin_addr.s_addr >> 24) & 0xFF);

			ThreadPool::QueueJob([=]
			{
				if (!ServerRunning)
					return;

				auto sock = ClientSocket;
				auto addr = ClientAddr.sin_addr;

				ClientCallBack(ThreadParameter, sock, addr);

				sceNetSocketClose(sock);
			});

			// Reset ClientSocket.
			ClientSocket = -1;
		}
	}

Cleanup:
	if (Verbose)
		Logger::Info("Listener Thread Exiting!\n");

	// Clean up.
	if (ServerSocket != -1)
		sceNetSocketClose(ServerSocket);
}

SocketListener::SocketListener(void(*clientCallBack)(void* tdParam, SceNetId Sock, SceNetInAddr sin_addr), void(*ConnectionLost)(void* tdParam), void(*ConnectionRestored)(void* tdParam), void* threadParameter, unsigned short Port, int timeOutSeconds, int protocol, bool verbose)
{
	ClientCallBack = clientCallBack;
	ThreadParameter = threadParameter;
	Verbose = verbose;
	ServerRunning = true;

	// Start up the main listener job.
	ThreadPool::QueueJob([=] { ListenThread(protocol, Port, timeOutSeconds); });

	// Create a thread for watching the network adapter so we dont hang the console.
	ThreadPool::QueueJob([=]
	{
		while (true)
		{
			int state = 0;
			sceNetCtlGetState(&state);

			switch (state)
			{
			default:
			case SCE_NET_CTL_STATE_IPOBTAINING:
			case SCE_NET_CTL_STATE_CONNECTING:
			case SCE_NET_CTL_STATE_DISCONNECTED:

				if (ServerRunning)
				{
					ConnectionLost(ThreadParameter);

					if (Verbose)
						Logger::Info("Listener: Network Connection Lost.\n");

					ServerRunning = false;
				}

				break;

			case SCE_NET_CTL_STATE_IPOBTAINED:

				if (!ServerRunning)
				{
					ConnectionRestored(ThreadParameter);

					if (Verbose)
						Logger::Info("Listener: Network Connection Online.\n");

					ServerRunning = true;
					ThreadPool::QueueJob([=] { ListenThread(protocol, Port, timeOutSeconds); });
				}

				break;
			}

			sceKernelUsleep(100 * 1000);
		}
	});
}

SocketListener::~SocketListener()
{
	ServerRunning = false;
}