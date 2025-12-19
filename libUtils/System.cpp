#include "stdafx.h"
#include "System.h"

int GetSDKVersion(int* sdkVersion)
{
	size_t sdkVersionLength = sizeof(int);
	return sysctlbyname("kern.sdk_version", (char*)sdkVersion, &sdkVersionLength, nullptr, 0);
}

int GetUpdateVersion(int* updateVersion)
{
	size_t updateVersionLength = sizeof(int);
	return sysctlbyname("machdep.upd_version", (char*)&updateVersion, &updateVersionLength, nullptr, 0);
}

int GetConsoleName(std::string& name)
{
	char buf[0x100];
	int ret = sceSystemServiceParamGetString(SCE_SYSTEM_SERVICE_PARAM_ID_SYSTEM_NAME, buf, sizeof(buf));

	if (ret == 0)
		name = buf;

	return ret;
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

int GetPsId(std::vector<uint8_t>& psid)
{
	size_t len = 16;
	psid.resize(len);
	return sysctlbyname("machdep.openpsid", psid.data(), &len, nullptr, 0);
}

int GetIdPs(std::vector<uint8_t>& idps)
{
	size_t len = 16;
	idps.resize(len);
	return sysctlbyname("machdep.idps", idps.data(), &len, nullptr, 0);
}

int ChangeSystemState(NewSystemState state)
{
	int ret = 0;

	switch (state)
	{
	case Suspend:
		ret = sceSystemStateMgrEnterStandby();
		break;

	case Shutdown:
		ret = sceSystemStateMgrTurnOff();
		break;

	case Reboot:
		ret = sceSystemStateMgrReboot();
		break;
	}

	return ret;
}

void RingBuzzer(BuzzerType Type)
{
	if (Type < 5)
		sceKernelIccSetBuzzer(Type);
}

void SetConsoleLED(ConsoleLEDColours Colour)
{
	switch (Colour)
	{
	default:
	case white:
		sceKernelIccIndicatorBootDone();
		break;

	case whiteBlinking:
		sceKernelIccIndicatorShutdown();
		break;

	case BlueBlinking:
		sceKernelIccIndicatorStandbyBoot();
		break;
	}
}