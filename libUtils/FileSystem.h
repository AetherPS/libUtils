#pragma once

class FileSystem
{
public:
	static bool Exists(const std::string filePath);
	static size_t GetSize(const std::string filePath);
	static bool Rename(std::string from, std::string to);
	static bool Remove(std::string filePath);
	static bool IsDirectoryEmpty(const std::string& dirPath);
	static std::vector<SceKernelDirent> GetDentList(const std::string& basePath);
	static bool MakeDir(const char* Dir, ...);
	static void CopyFile(const char* File, const char* Destination);

	static bool Read(const std::string& filePath, void* data, size_t length);
	static bool Write(const std::string& filePath, const void* data, size_t length);

private:

};