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

int32_t GetCPUTemp();
int32_t GetSOCTemp();
int GetSDKVersion();
int GetUpdateVersion();
std::string GetConsoleName();
ConsoleTypes GetConsoleType();
std::string GetPsId();
std::string GetIdPs();
int ChangeSystemState(NewSystemState state);
std::tuple<uint64_t, uint64_t> GetStorageStats();
void RingBuzzer(BuzzerType Type);
void SetConsoleLED(ConsoleLEDColours Colour);