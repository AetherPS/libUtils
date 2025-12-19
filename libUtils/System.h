#pragma once

enum ConsoleTypes
{
	CT_UNK,
	CT_DIAG,	//0x80
	CT_DEVKIT,	//0x81
	CT_TESTKIT, //0x82
	CT_RETAIL,	//0x83 -> 0x8F
	CT_KRATOS,	//0xA0 IMPOSSIBLE??
};

enum ConsoleLEDColours
{
	white,
	whiteBlinking,
	BlueBlinking,
};

enum NewSystemState
{
	Suspend = 1,
	Shutdown = 2,
	Reboot = 3,
};

int GetSDKVersion(int* sdkVersion);
int GetUpdateVersion(int* updateVersion);
int GetConsoleName(std::string& name);
ConsoleTypes GetConsoleType();
int GetPsId(std::vector<uint8_t>& psid);
int GetIdPs(std::vector<uint8_t>& idps);
int ChangeSystemState(NewSystemState state);
void RingBuzzer(BuzzerType Type);
void SetConsoleLED(ConsoleLEDColours Colour);