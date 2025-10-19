#pragma once

bool Jailbreak(uint64_t authId, bool nullSandboxPath);
bool RestoreJail();
int LoadSprx(int processId, std::string libraryPath);
bool UnloadSprx(int processId, int libraryHandle);
