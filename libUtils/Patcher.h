#pragma once

class Patcher
{
public:
	void SetPatch();
	void RestorePatch();

	Patcher(uint64_t Address, const void* Data, size_t Length, bool setNow = true);
	~Patcher();

private:
	uint64_t Address;
	std::unique_ptr<uint8_t[]> OriginalData;
	std::unique_ptr<uint8_t[]> PatchData;
	size_t Length;
};