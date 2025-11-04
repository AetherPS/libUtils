#pragma once

extern uint64_t BaseAddress;

uint64_t ResolveAddress(uint64_t offset);
void* ResolveAddressv(uint64_t offset);
bool IsInTextSeg(uint64_t addr);
bool IsMemoryMapped(uint64_t address);