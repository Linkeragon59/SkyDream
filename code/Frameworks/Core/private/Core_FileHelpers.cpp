#include "Core_FileHelpers.h"

#include "Core_Facade.h"

#include <sys/stat.h>
#include <fstream>
#include <iostream>

namespace FileHelpers
{
	bool FileExists(const char* aFilePath)
	{
		struct stat buffer;
		return (stat(aFilePath, &buffer) == 0);
	}

	bool ReadAsBuffer(const char* aFilePath, std::vector<char>& anOutBuffer)
	{
		std::ifstream file(aFilePath, std::ios::ate | std::ios::binary);
		if (!file.is_open())
		{
			std::cout << "Failed to read the file %s" << aFilePath << std::endl;
			return false;
		}
		std::streamsize fileSize = file.tellg();
		anOutBuffer.resize(fileSize);
		file.seekg(0);
		file.read(anOutBuffer.data(), fileSize);
		file.close();
		return true;
	}

	bool ReadAsString(const char* aFilePath, std::string& anOutString)
	{
		std::ifstream file(aFilePath, std::ios::ate);
		if (!file.is_open())
		{
			std::cout << "Failed to read the file %s" << aFilePath << std::endl;
			return false;
		}
		std::streamsize fileSize = file.tellg();
		anOutString.resize(fileSize);
		file.seekg(0);
		file.read(anOutString.data(), fileSize);
		file.close();
		return true;
	}
}
