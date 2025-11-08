#include "stdafx.h"
#include "Process.h"

int GetProcessList(std::vector<kinfo_proc>& ProcessList)
{
	size_t length;
	static int name[] = { CTL_KERN, KERN_PROC, KERN_PROC_PROC, 0 };

	// Get the size of buffer needed.
	if (sysctl(name, 3, nullptr, &length, nullptr, 0) < 0)
		return -1;

	// Resize our vector to accommodate.
	try
	{
		ProcessList.resize(length / sizeof(kinfo_proc));
	}
	catch (const std::bad_alloc&)
	{
		return -1;
	}

	// Retrive the processes.
	if (sysctl(name, 3, ProcessList.data(), &length, nullptr, 0) < 0)
		return -1;

	// Fix names.
	for (auto& proc : ProcessList)
	{
		sceKernelGetProcessName(proc.ki_pid, (char*)proc.ki_comm);
	}

	return 0;
}

int GetProcInfo(int pid, kinfo_proc* out)
{
	size_t length = sizeof(kinfo_proc);
	static int name[] = { CTL_KERN, KERN_PROC, KERN_PROC_PID, pid };

	// Retrive the processes.
	if (sysctl(name, 4, out, &length, nullptr, 0) < 0)
		return -1;

	// Fix name.
	sceKernelGetProcessName(out->ki_pid, (char*)out->ki_comm);

	return 0;
}

bool MountProcFs()
{
	if (sceKernelMkdir("/mnt/proc", 0777) != 0)
	{
		Logger::Error("Failed to make procfs dir.");
		return false;
	}

	if (mount("procfs", "/mnt/proc", 0, NULL) < 0)
	{
		Logger::Error("Failed to mount procfs.");
		return false;
	}

	return true;
}

int GetPidByName(const char* ProcessName)
{
	// Get the list of running processes.
	std::vector<kinfo_proc> processList;
	GetProcessList(processList);

	// Get the pid of the process.
	for (const auto& i : processList)
	{
		if (strstr(i.ki_comm, ProcessName))
			return i.ki_pid;
	}

	return -1;
}

int GetPidByAppId(int appId)
{
	int pid = 0;

	// Get the list of running processes.
	std::vector<kinfo_proc> processList;
	GetProcessList(processList);

	for (const auto& i : processList)
	{
		// Get the app info using the pid.
		SceAppInfo appInfo;
		sceKernelGetAppInfo(i.ki_pid, &appInfo);

		// Using the titleId match our desired app and return the pid from the kinfo_proc.
		if (appInfo.AppId == appId)
		{
			pid = i.ki_pid;

			break;
		}
	}

	return pid;
}

int GetPidByTitleId(const char* TitleId)
{
	int pid = 0;

	// Get the list of running processes.
	std::vector<kinfo_proc> processList;
	GetProcessList(processList);

	for (const auto& i : processList)
	{
		// Get the app info using the pid.
		SceAppInfo appInfo;
		sceKernelGetAppInfo(i.ki_pid, &appInfo);

		// Using the titleId match our desired app and return the pid from the kinfo_proc.
		if (!strcmp(appInfo.TitleId, TitleId))
		{
			pid = i.ki_pid;

			break;
		}
	}

	return pid;
}

std::string GetSandboxPath(int pid)
{
	// Easy way to get the titleId.
	SceAppInfo info;
	if (sceKernelGetAppInfo(pid, &info) != 0)
	{
		Logger::Error("Failed to get appinfo for pid %d.", pid);
		return "";
	}

	// Find all of the sandbox paths since some can linger if they fail to clean up. ex. AAAA#####_##X With X being incremented for every failed cleanup.
	std::vector<std::string> sandboxList;
	auto entries = FileSystem::GetDentList("/mnt/sandbox");
	for (const auto& entry : entries)
	{
		std::string name(entry.d_name);
		if (name.find(info.TitleId) != std::string::npos)
		{
			sandboxList.push_back(name);
		}
	}

	// Find the real sandbox path for the current proc.
	std::string sandboxPath;
	for (const auto& sandbox : sandboxList)
	{
		auto tempPath = va("/mnt/sandbox/%s", sandbox.c_str());
		auto entries = FileSystem::GetDentList(tempPath);

		if (entries.size() < 3)
			continue;

		sandboxPath = tempPath;
	}

	return sandboxPath;
}

std::string GetSandboxRandom(int pid)
{
	auto sandboxPath = GetSandboxPath(pid);

	std::string result;
	auto entries = FileSystem::GetDentList(sandboxPath);
	for (const auto& entry : entries)
	{
		std::string folderName(entry.d_name);

		// Skip '.' and '..' directories
		if (entry.d_type != DT_DIR || folderName == "." || folderName == "..")
		{
			continue;
		}

		// Regex pattern for random string of length 10
		if (!std::regex_match(folderName, std::regex("^[A-Za-z0-9]{10}$")))
		{
			continue;
		}

		result = folderName;
	}

	return result;
}