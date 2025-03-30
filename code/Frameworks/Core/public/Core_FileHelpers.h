#pragma once

namespace FileHelpers
{
	bool FileExists(const char* aFilePath);
	bool ReadAsBuffer(const char* aFilePath, std::vector<char>& anOutBuffer);
	bool ReadAsString(const char* aFilePath, std::string& anOutString);
}
