#include "stdafx.h"
#include "AppControl.h"

void ExitGraceful()
{
	SceAppInfo info;
	sceKernelGetAppInfo(getpid(), &info);

	sceLncUtilKillApp(info.AppId);
}

std::string GetBigAppTitleId()
{
	auto appId = sceSystemServiceGetAppIdOfBigApp();

	if (appId < 0)
		return "N/A";

	SceAppInfo info;
	if (sceApplicationGetAppInfoByAppId(appId, &info) != 0)
		return "N/A";

	return std::string(info.TitleId);
}

int GetAppIdByTitleId(const char* TitleId)
{
	int appId = 0;

	// Get the list of running processes.
	std::vector<kinfo_proc> processList;
	GetProcessList(processList);

	for (const auto& i : processList)
	{
		// Get the app info using the pid.
		SceAppInfo appInfo;
		sceKernelGetAppInfo(i.ki_pid, &appInfo);

		// Using the titleId match our desired app and return the appId from the appinfo.
		if (!strcmp(appInfo.TitleId, TitleId))
		{
			appId = appInfo.AppId;

			break;
		}
	}

	return appId;
}

int StartApp(const char* titleId, char** args, int userId)
{
	auto appId = GetAppIdByTitleId(titleId);
	if (appId)
	{
		Logger::Warn("App %s is already running.", titleId);
		return 0;
	}

	// Set up the launch paramaters with the given userId as the owner/launcher.
	LaunchAppParam appParam;
	appParam.size = sizeof(LaunchAppParam);
	appParam.userId = userId;
	appParam.enableCrashReport = 0;
	appParam.checkFlag = 0;
	appParam.appAttr = 0;

	// Launch the app.
	auto res = sceLncUtilLaunchApp(titleId, args, &appParam);
	if (res != 0)
	{
		Logger::Error("Failed to start app: %llX", res);
	}

	return res;
}

int StartRestartApp(const char* titleId, char** args, int userId)
{
	auto appId = GetAppIdByTitleId(titleId);
	if (appId)
	{
		// App is already running kill it so we can restart it.
		sceLncUtilKillApp(appId);
	}

	// Set up the launch paramaters with the given userId as the owner/launcher.
	LaunchAppParam appParam;
	appParam.size = sizeof(LaunchAppParam);
	appParam.userId = userId;
	appParam.enableCrashReport = 0;
	appParam.checkFlag = 0;
	appParam.appAttr = 0;

	// Launch the app.
	auto res = sceLncUtilLaunchApp(titleId, args, &appParam);
	if (res <= 0)
	{
		Logger::Error("Failed to start app: %llX", res);
	}

	return res;
}

int StopApp(const char* titleId)
{
	auto appId = GetAppIdByTitleId(titleId);
	if (appId == 0)
	{
		Logger::Error("App %s is not running.", titleId);
		return -1;
	}

	auto res = sceLncUtilKillApp(appId);
	if (res != 0)
	{
		Logger::Error("Failed to stop app %s: %llX", titleId, res);
	}

	return res;
}