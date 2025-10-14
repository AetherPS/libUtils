#include "stdafx.h"
#include "StringExt.h"

bool BeginsWith(const std::string& text, const std::string& search)
{
	return (text.size() >= search.size() && text.substr(0, search.size()) == search);
}

bool EndsWith(const std::string& text, const std::string& search)
{
	return (text.size() >= search.size() && text.substr(text.size() - search.size(), search.size()) == search);
}

std::string ToLower(std::string str)
{
	std::transform(str.begin(), str.end(), str.begin(), ::tolower);
	return str;
}

std::string ToUpper(std::string str)
{
	std::transform(str.begin(), str.end(), str.begin(), ::toupper);
	return str;
}

std::string ByteArrayToHexString(const std::vector<unsigned char>& byteArray)
{
	std::string hexString;
	hexString.reserve(byteArray.size() * 2); // Reserve space for efficiency

	for (const auto& byte : byteArray)
	{
		hexString += "0123456789ABCDEF"[byte >> 4];   // High nibble
		hexString += "0123456789ABCDEF"[byte & 0x0F]; // Low nibble
	}

	return hexString;
}

std::vector<uint8_t> HexStringToByteArray(const std::string& hexStr)
{
	std::vector<uint8_t> bytes;
	for (size_t i = 0; i < hexStr.length(); i += 2)
	{
		std::string byteString = hexStr.substr(i, 2);
		uint8_t byteValue = std::stoi(byteString, nullptr, 16);
		bytes.push_back(byteValue);
	}
	return bytes;
}

std::string GetFileName(const std::string& filePath)
{
	size_t lastSlashPos = filePath.find_last_of("/\\");
	std::string onlyFilename = filePath.substr(lastSlashPos + 1);
	return onlyFilename;
}

std::string GetResourceName(const std::string& filePath)
{
	size_t lastDotPos = filePath.find_last_of(".");

	if (lastDotPos != std::string::npos)
	{
		size_t secondLastDotPos = filePath.rfind(".", lastDotPos - 1);
		if (secondLastDotPos != std::string::npos)
		{
			// Found the position of the second last dot, extract the substring from there to the end
			return filePath.substr(secondLastDotPos + 1);
		}
	}

	// If no second last dot is found, return the whole string as the filename
	return filePath;
}

std::string GetTimeNow()
{
	// Get the current time.
	std::time_t currentTime = std::time(nullptr);
	struct std::tm* localTime = std::localtime(&currentTime);

	// Format the timestamp.
	std::string formattedTime = std::to_string(localTime->tm_hour % 12 == 0 ? 12 : localTime->tm_hour % 12);
	formattedTime += ":";
	formattedTime += std::to_string(localTime->tm_min);
	formattedTime += ":";
	formattedTime += std::to_string(localTime->tm_sec);
	formattedTime += " ";
	formattedTime += (localTime->tm_hour >= 12 ? "PM" : "AM");

	return formattedTime;
}