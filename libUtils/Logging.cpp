#include "stdafx.h"
#include "Logging.h"

void Logging::Init()
{
	if (Initialized)
		return;

	// Open a file handle to all the tty channels.
	for (int i = 0; i < 5; i++)
	{
		// Open the tty channel.
		TTY[i] = fopen(va("/dev/deci_tty%i", i + 2).c_str(), "w");

		// If we cant open one abort.
		if (!TTY[i])
		{
			printf("Failed to open /dev/deci_tty%i\n", i + 2);
			return;
		}
	}

	Initialized = true;
}

void Logging::Term()
{
	if (!Initialized)
		return;

	// Free up all the file handles.
	for (int i = 0; i < 5; i++)
	{
		fclose(TTY[i]);
	}

	Initialized = false;
}

void Logging::Log(int channel, const char* fmt, ...)
{
	// Make sure we're initialized.
	if (!Initialized)
	{
		return;
	}

	// Start the va list.
	va_list args;
	va_start(args, fmt);

	// Set the default channel to stdout.
	auto handle = &_Stdout;

	// If we have a valid channel set it now.
	if (channel >= 2 && channel <= 7)
		handle = TTY[channel - 2];

	// Print to the desired channel.
	if (vfprintf(handle, fmt, args) > 0)
	{
		// If we actually were able to print flush the buffer now!.
		fflush(handle);
	}

	// end the va list.
	va_end(args);
}

void Logging::Log(const char* fmt, ...)
{
	// Make sure we're initialized.
	if (!Initialized)
	{
		return;
	}

	// Start the va list.
	va_list args;
	va_start(args, fmt);

	// Print to the value.
	if (vfprintf(&_Stdout, fmt, args) > 0)
	{
		// If we actually were able to print flush the buffer now!.
		fflush(&_Stdout);
	}

	// end the va list.
	va_end(args);
}

void Logging::LogHardware(const char* fmt, ...)
{
	char buffer[0x2000];

	//Create full string from va list.
	va_list args;
	va_start(args, fmt);
	vsprintf(buffer, fmt, args);
	va_end(args);

	sceKernelDebugOutText(0, buffer);
}

void klog(const char* fmt, ...)
{
	char Buffer[0x200];

	//Create full string from va list.
	va_list args;
	va_start(args, fmt);
	vsprintf(Buffer, fmt, args);
	va_end(args);

	sceKernelDebugOutText(0, Buffer);
}

void hexdump(void* ptr, int buflen, bool showAddress)
{
	unsigned char* buf = (unsigned char*)ptr;
	int i, j;
	for (i = 0; i < buflen; i += 16)
	{
		showAddress ? klog("%06x: ", (uint64_t)ptr + i) : klog("%06x: ", i);
		for (j = 0; j < 16; j++)
			if (i + j < buflen)
				klog("%02x ", buf[i + j]);
			else
				klog("   ");
		klog(" ");
		for (j = 0; j < 16; j++)
			if (i + j < buflen)
				klog("%c", isprint(buf[i + j]) ? buf[i + j] : '.');
		klog("\n");
	}
}