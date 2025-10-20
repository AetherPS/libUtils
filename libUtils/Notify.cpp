#include "stdafx.h"
#include "Notify.h"

void NotifyCustom(const char* icon, const char* fmt, ...)
{
	SceNotificationRequest Buffer;

	//Create full string from va list.
	va_list args;
	va_start(args, fmt);
	vsprintf(Buffer.message, fmt, args);
	va_end(args);

	//Populate the notify buffer.
	Buffer.type = SceNotificationRequestType::NotificationRequest; //this one is just a standard one and will print what ever is stored at the buffer.Message.
	Buffer.unk3 = 0;
	Buffer.useIconImageUri = 1; //Bool to use a custom uri.
	Buffer.targetId = -1; //Not sure if name is correct but is always set to -1.
	strcpy(Buffer.iconUri, icon); //Copy the uri to the buffer.

	//From user land we can call int64_t sceKernelSendNotificationRequest(int64_t unk1, char* Buffer, size_t size, int64_t unk2) which is a libkernel import.
	sceKernelSendNotificationRequest(0, &Buffer, 3120, 0);
}

void Notify(const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	NotifyCustom("cxml://psnotification/tex_default_icon_tips", fmt, args);

	va_end(args);
}