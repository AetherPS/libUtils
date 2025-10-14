#pragma once

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wformat-security"
template<typename ... Args>
std::string va(const std::string& format, Args ... args)
{
    const int size_s = std::snprintf(nullptr, 0, format.c_str(), args ...) + 1; // Extra space for '\0'
    if (size_s <= 0) { return "Error"; }
    std::vector<char> buf(size_s);
    std::snprintf(buf.data(), size_s, format.c_str(), args ...);
    return std::string(buf.data(), buf.data() + size_s - 1); // We don't want the '\0' inside
}
#pragma clang diagnostic pop

bool BeginsWith(const std::string& text, const std::string& search);
bool EndsWith(const std::string& text, const std::string& search);
std::string ToLower(std::string str);
std::string ToUpper(std::string str);
std::string ByteArrayToHexString(const std::vector<unsigned char>& byteArray);
std::vector<uint8_t> HexStringToByteArray(const std::string& hexStr);
std::string GetFileName(const std::string& filePath);
std::string GetResourceName(const std::string& filePath);
std::string GetTimeNow();