#include "stdafx.h"
#include "System.h"

int32_t GetCPUTemp()
{
	int32_t CPUTemp = 0;
	if (sceKernelGetCpuTemperature(&CPUTemp) == 0)
		return CPUTemp;
	else
		return -1;
}

int32_t GetSOCTemp()
{
	int32_t SOCTemp = 0;
	if (sceKernelGetSocSensorTemperature(0, &SOCTemp) == 0)
		return SOCTemp;
	else
		return -1;
}

int GetSDKVersion()
{
	int sdk_version = 0;
	size_t sdk_versionlen = 4;

	sysctlbyname("kern.sdk_version", (char*)&sdk_version, &sdk_versionlen, nullptr, 0);

	return sdk_version;
}

int GetUpdateVersion()
{
	int sdk_version = 0;
	size_t sdk_versionlen = 4;

	sysctlbyname("machdep.upd_version", (char*)&sdk_version, &sdk_versionlen, nullptr, 0);

	return sdk_version;
}

std::string GetConsoleName()
{
	char buffer[0x100];
	if (sceSystemServiceParamGetString(SCE_SYSTEM_SERVICE_PARAM_ID_SYSTEM_NAME, buffer, sizeof(buffer)) == 0)
		return buffer;
	else
		return "Error";
}

ConsoleTypes GetConsoleType()
{
	char IDPS[16] = { 0 };
	size_t IDPSlen = 16;
	sysctlbyname("machdep.idps", (char*)&IDPS, &IDPSlen, nullptr, 0);

	switch (IDPS[5] & 0xffU)
	{
	case 0x80:
		return CT_DIAG;
		break;

	case 0x81:
		return CT_DEVKIT;
		break;

	case 0x82:
		return CT_TESTKIT;
		break;

	case 0x83 ... 0x8F:
		return CT_RETAIL;
		break;

	case 0xA0:
		return CT_KRATOS;
		break;
	}

	return CT_UNK;
}

std::string GetPsId()
{
	size_t psidlen = 16;
	std::vector<uint8_t> psidRaw;
	psidRaw.resize(psidlen);
	if (sysctlbyname("machdep.openpsid", psidRaw.data(), &psidlen, NULL, 0) == 0)
		return ByteArrayToHexString(psidRaw);
	else
		return "Error";
}

std::string GetIdPs()
{
	size_t psidlen = 16;
	std::vector<uint8_t> psidRaw;
	psidRaw.resize(psidlen);
	if (sysctlbyname("machdep.idps", psidRaw.data(), &psidlen, NULL, 0) == 0)
		return ByteArrayToHexString(psidRaw);
	else
		return "Error";
}
