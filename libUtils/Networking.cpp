#include "stdafx.h"
#include "Networking.h"

std::string GetMacAddress(int ifNameNum)
{
	SceNetIfEntry ifEntry;
	auto res = sceNetGetIfList((SceNetIfName)ifNameNum, &ifEntry, 1);
	if (res < 0)
	{
		Logger::Error("%s: failed to get IfList for %i - 0x%llX\n", __FUNCTION__, ifNameNum, res);
		return "N/A";
	}

	char buffer[20];
	res = sceNetEtherNtostr((SceNetEtherAddr*)ifEntry.MacAddress, buffer, 20);
	if (res < 0)
	{
		Logger::Error("%s: failed to get IfList for %i - 0x%llX\n", __FUNCTION__, ifNameNum, res);
		return "N/A";
	}

	return buffer;
}