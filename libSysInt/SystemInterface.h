#pragma once

bool Jailbreak(uint64_t authId = -1, bool nullSandboxPath = false);
bool RestoreJail();
int LoadSprx(int processId, std::string libraryPath);
bool UnloadSprx(int processId, int libraryHandle);
