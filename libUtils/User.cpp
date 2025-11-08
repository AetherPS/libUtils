#include "stdafx.h"
#include "User.h"

int GetForeGroundUserId()
{
	int userId;
	sceUserServiceGetForegroundUser(&userId);
	return userId;
}

std::string GetForegroundUserName()
{
	char OnlineName[0x20];
	int ForegroundAccountId = 0;
	if (sceUserServiceGetForegroundUser(&ForegroundAccountId) == 0 && ForegroundAccountId != 0)
	{
		if (sceUserServiceGetUserName(ForegroundAccountId, OnlineName, 0x20) != 0)
		{
			strcpy(OnlineName, "Unknown");
		}
	}

	return OnlineName;
}