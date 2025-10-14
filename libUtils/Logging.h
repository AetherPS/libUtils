#pragma once

#define DARK_PURP_TEXT "\x1b[38;5;103m"
#define PURP_TEXT "\x1b[38;5;61m"
#define RESET_COLOR "\x1b[0m"

class Logging
{
public:
	static void Init();
	static void Term();

	static void Log(int channel, const char* fmt, ...);
	static void Log(const char* fmt, ...);
	static void LogHardware(const char* fmt, ...);

private:
	static inline bool Initialized;
	static inline FILE* TTY[5];
};

void klog(const char* fmt, ...);
void hexdump(void* ptr, int buflen, bool showAddress = false);