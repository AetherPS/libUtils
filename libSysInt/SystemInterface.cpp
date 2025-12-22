#include <stdint.h>
#include <string>
#include <GoldHEN.h>
#include <FusionDriver.h>
#include <KernelExt.h>
#include <RemoteSprx.h>

#include "SystemInterface.h"

// Jailbackups
JailBackup FusionBackup;
bool FusionBackupUsed;
jailbreak_backup GoldHenBackup;
bool GoldHenBackupUsed;

bool Jailbreak(uint64_t authId, bool nullSandboxPath)
{
	if (Fusion::IsDriverLoaded()) // Try Fusion first since we this would be prefered.
	{
		FusionBackupUsed = true;
		GoldHenBackupUsed = false;
		return Fusion::Jailbreak(getpid(), &FusionBackup, authId, nullSandboxPath) == 0;
	}
	else // If we dont have fusion fall back on the GoldHen Jailbreak.
	{
		FusionBackupUsed = false;
		GoldHenBackupUsed = true;
		return (sys_sdk_jailbreak(&GoldHenBackup) == 0);
	}
}

bool RestoreJail()
{
	if (Fusion::IsDriverLoaded() && FusionBackupUsed) // Try Fusion first since we this would be prefered.
	{
		return Fusion::RestoreJail(getpid(), FusionBackup) == 0;
	}
	else if (GoldHenBackupUsed) // If we dont have fusion fall back on GoldHen.
	{
		return (sys_sdk_unjailbreak(&GoldHenBackup) == 0);
	}

	// No backup stored.
	return false;
}

int LoadSprx(int processId, std::string libraryPath)
{
	if (Fusion::IsDriverLoaded()) // Try Fusion first since we this would be prefered.
	{
		return Fusion::LoadSprx(processId, libraryPath.c_str());
	}
	else
	{
		// GoldHen is sort of dumb for this but we need the process name not pid...
		char processName[32];
		sceKernelGetProcessName(processId, processName);

		return sys_sdk_proc_prx_load(processName, libraryPath.data());
	}
}

bool UnloadSprx(int processId, int libraryHandle)
{
	if (Fusion::IsDriverLoaded()) // Try Fusion first since we this would be prefered.
	{
		return Fusion::UnloadSprx(processId, libraryHandle);
	}
	else
	{
		// GoldHen is sort of dumb for this but we need the process name not pid...
		char processName[32];
		sceKernelGetProcessName(processId, processName);

		return sys_sdk_proc_prx_unload(processName, libraryHandle);
	}
}