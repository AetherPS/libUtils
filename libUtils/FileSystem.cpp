#include "stdafx.h"
#include "FileUtils.h"
#include "FileSystem.h"

bool FileSystem::Exists(const std::string filePath)
{
	int fd = sceKernelOpen(filePath.c_str(), SCE_KERNEL_O_RDONLY, 0);
	if (fd < 0)
	{
		// File does not exist or cannot be opened
		return false;
	}

	sceKernelClose(fd);
	return true;
}

size_t FileSystem::GetSize(const std::string filePath)
{
	int fd = sceKernelOpen(filePath.c_str(), SCE_KERNEL_O_RDONLY, 0);
	if (fd < 0)
	{
		Logger::Error("%s: Failed to open file %s: %d %08X\n",
			__FUNCTION__, filePath.c_str(), fd, sceKernelError(*__error()));
		return fd;
	}

	SceKernelStat stat;
	auto res = sceKernelFstat(fd, &stat);
	if (res != 0)
	{
		Logger::Error("%s: Failed to stat file %s: %08X\n",
			__FUNCTION__, filePath.c_str(), sceKernelError(*__error()));
		sceKernelClose(fd);
		return res;
	}

	sceKernelClose(fd);
	return static_cast<size_t>(stat.st_size);
}

bool FileSystem::Rename(std::string from, std::string to)
{
	int res = sceKernelRename(from.c_str(), to.c_str());

	if (res != 0)
	{
		Logger::Error("%s Failed to rename file %s to %s: ret=%d errno=%d sceKernelError=%08X\n",
			__FUNCTION__, from.c_str(), to.c_str(), res, *__error(), sceKernelError(*__error()));
		return false;
	}

	return true;
}
bool FileSystem::Remove(std::string filePath)
{
	int res = sceKernelUnlink(filePath.c_str());
	if (res != 0)
	{
		Logger::Error("%s Failed to remove file %s: ret=%d errno=%d sceKernelError=%08X\n",
			__FUNCTION__, filePath.c_str(), res, *__error(), sceKernelError(*__error()));
		return false;
	}

	return true;
}

bool FileSystem::IsDirectoryEmpty(const std::string& dirPath)
{
	int fd = sceKernelOpen(dirPath.c_str(), SCE_KERNEL_O_RDONLY | O_DIRECTORY, 0);
	if (fd < 0)
	{
		Logger::Error("%s: Failed to open directory %s: %d %08X\n",
			__FUNCTION__, dirPath.c_str(), fd, sceKernelError(*__error()));
		return false;
	}

	const size_t bufferSize = 0x4000;
	char buffer[bufferSize];

	ssize_t bytesRead = sceKernelGetdents(fd, buffer, bufferSize);
	if (bytesRead < 0)
	{
		Logger::Error("%s: sceKernelGetdents failed for %s: %zd %08X\n",
			__FUNCTION__, dirPath.c_str(), bytesRead, sceKernelError(*__error()));
		sceKernelClose(fd);
		return false;
	}

	bool isEmpty = true;
	size_t offset = 0;
	while (offset < static_cast<size_t>(bytesRead))
	{
		SceKernelDirent* entry = reinterpret_cast<SceKernelDirent*>(buffer + offset);
		offset += entry->d_reclen;

		// Skip "." and ".."
		if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
		{
			isEmpty = false;
			break;
		}
	}

	sceKernelClose(fd);
	return isEmpty;
}

std::vector<SceKernelDirent> FileSystem::GetDentList(const std::string& basePath)
{
	std::vector<SceKernelDirent> entries;
	int fd = sceKernelOpen(basePath.c_str(), SCE_KERNEL_O_RDONLY | O_DIRECTORY, 0);
	if (fd < 0)
	{
		Logger::Error("%s: Failed to open directory %s: %d %08X\n",
			__FUNCTION__, basePath.c_str(), fd, sceKernelError(*__error()));
		return entries;
	}

	const size_t bufferSize = 0x4000;
	char buffer[bufferSize];

	ssize_t bytesRead;
	while ((bytesRead = sceKernelGetdents(fd, buffer, bufferSize)) > 0)
	{
		size_t offset = 0;
		while (offset < static_cast<size_t>(bytesRead))
		{
			SceKernelDirent* dirent = reinterpret_cast<SceKernelDirent*>(buffer + offset);
			offset += dirent->d_reclen;

			// Add a copy to vector
			entries.push_back(*dirent);
		}
	}

	if (bytesRead < 0)
	{
		Logger::Error("%s: Error reading directory %s: %zd %08X\n",
			__FUNCTION__, basePath.c_str(), bytesRead, sceKernelError(*__error()));
	}

	sceKernelClose(fd);
	return entries;
}

bool FileSystem::MakeDir(const char* Dir, ...)
{
	char buffer[PATH_MAX] = { 0 };
	va_list args;
	va_start(args, Dir);
	vsnprintf(buffer, sizeof(buffer), Dir, args);
	va_end(args);

	int res = sceKernelMkdir(buffer, 0777);
	if (res != 0)
	{
		// Directory already exists
		if (res == 0x80020011)
		{
			Logger::Warn("%s: Directory already exists: %s\n", __FUNCTION__, buffer);
			return true;
		}

		Logger::Error("%s: Failed to create directory %s: 0x%08X %d\n",
			__FUNCTION__, buffer, res, sceKernelError(*__error()));
		return false;
	}

	return true;
}

void FileSystem::CopyFile(const char* File, const char* Destination)
{
	int src = -1, dst = -1;
	SceKernelStat Stats;

	// Open source file
	src = sceKernelOpen(File, SCE_KERNEL_O_RDONLY, 0);
	if (src < 0)
	{
		Logger::Error("%s Failed to open source file %s: %d %d\n",
			__FUNCTION__, File, src, sceKernelError(*__error()));
		return;
	}

	// Open destination file
	dst = sceKernelOpen(Destination, SCE_KERNEL_O_CREAT | SCE_KERNEL_O_WRONLY | SCE_KERNEL_O_TRUNC, 0777);
	if (dst < 0)
	{
		Logger::Error("%s Failed to open destination file %s: %d %d\n",
			__FUNCTION__, Destination, dst, sceKernelError(*__error()));
		sceKernelClose(src);
		return;
	}

	// Get file stats
	int fstatResult = sceKernelFstat(src, &Stats);
	if (fstatResult < 0)
	{
		Logger::Error("%s Failed to stat source file %s: %d %d\n",
			__FUNCTION__, File, fstatResult, sceKernelError(*__error()));
		sceKernelClose(src);
		sceKernelClose(dst);
		return;
	}

	if (Stats.st_size <= 0)
	{
		Logger::Error("%s Invalid file size for %s: %lld\n",
			__FUNCTION__, File, (long long)Stats.st_size);
		sceKernelClose(src);
		sceKernelClose(dst);
		return;
	}

	// Allocate buffer
	char* FileData = (char*)malloc(Stats.st_size);
	if (!FileData)
	{
		Logger::Error("%s Failed to allocate buffer for %s size %lld\n",
			__FUNCTION__, File, (long long)Stats.st_size);
		sceKernelClose(src);
		sceKernelClose(dst);
		return;
	}

	// Read loop
	ssize_t totalRead = 0;
	while (totalRead < Stats.st_size)
	{
		ssize_t r = sceKernelRead(src, FileData + totalRead, Stats.st_size - totalRead);
		if (r <= 0)
		{
			Logger::Error("%s Failed to read from %s: %zd %d\n",
				__FUNCTION__, File, r, sceKernelError(*__error()));
			free(FileData);
			sceKernelClose(src);
			sceKernelClose(dst);
			return;
		}
		totalRead += r;
	}

	// Write loop
	ssize_t totalWritten = 0;
	while (totalWritten < totalRead)
	{
		ssize_t w = sceKernelWrite(dst, FileData + totalWritten, totalRead - totalWritten);
		if (w <= 0)
		{
			Logger::Error("%s Failed to write to %s: %zd %d\n",
				__FUNCTION__, Destination, w, sceKernelError(*__error()));
			free(FileData);
			sceKernelClose(src);
			sceKernelClose(dst);
			return;
		}
		totalWritten += w;
	}

	free(FileData);
	sceKernelClose(src);
	sceKernelClose(dst);
}


int FileSystem::Read(const std::string& filePath, void* data, size_t length)
{
	if (!data) return -1;

	int fd = sceKernelOpen(filePath.c_str(), SCE_KERNEL_O_RDONLY, 0);
	if (fd < 0)
	{
		Logger::Error("%s Failed to open file \"%s\": ret=%d errno=%d sceKernelError=%08X\n",
			__FUNCTION__, filePath.c_str(), fd, *__error(), sceKernelError(*__error()));
		return fd;
	}

	size_t totalRead = 0;
	while (totalRead < length)
	{
		ssize_t r = sceKernelRead(fd, static_cast<char*>(data) + totalRead, length - totalRead);
		if (r < 0)
		{
			Logger::Error("%s Failed to read from \"%s\": ret=%zd errno=%d sceKernelError=%08X\n",
				__FUNCTION__, filePath.c_str(), r, *__error(), sceKernelError(*__error()));
			sceKernelClose(fd);
			return static_cast<int>(r);
		}
		else if (r == 0) // EOF
		{
			break;
		}

		totalRead += r;
	}

	int closeResult = sceKernelClose(fd);
	if (closeResult < 0)
	{
		Logger::Error("%s Failed to close file \"%s\": ret=%d errno=%d sceKernelError=%08X\n",
			__FUNCTION__, filePath.c_str(), closeResult, *__error(), sceKernelError(*__error()));
		return closeResult;
	}

	return 0;
}

int FileSystem::Write(const std::string& filePath, const void* data, size_t length)
{
	int fd = sceKernelOpen(filePath.c_str(), SCE_KERNEL_O_CREAT | SCE_KERNEL_O_WRONLY | SCE_KERNEL_O_TRUNC, 0777);
	if (fd < 0)
	{
		Logger::Error("%s Failed to open file \"%s\": ret=%d errno=%d sceKernelError=%08X\n",
			__FUNCTION__, filePath.c_str(), fd, *__error(), sceKernelError(*__error()));
		return fd;
	}

	size_t totalWritten = 0;
	while (totalWritten < length)
	{
		ssize_t w = sceKernelWrite(fd, static_cast<const char*>(data) + totalWritten, length - totalWritten);
		if (w <= 0)
		{
			Logger::Error("%s Failed to write to \"%s\": ret=%zd errno=%d sceKernelError=%08X\n",
				__FUNCTION__, filePath.c_str(), w, *__error(), sceKernelError(*__error()));
			sceKernelClose(fd);
			return (w < 0) ? static_cast<int>(w) : -1;
		}
		totalWritten += w;
	}

	int closeResult = sceKernelClose(fd);
	if (closeResult < 0)
	{
		Logger::Error("%s Failed to close file \"%s\": ret=%d errno=%d sceKernelError=%08X\n",
			__FUNCTION__, filePath.c_str(), closeResult, *__error(), sceKernelError(*__error()));
		return closeResult;
	}

	return 0;
}