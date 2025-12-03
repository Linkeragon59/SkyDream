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

	bool ReadAsBuffer(const char* aFilePath, std::vector<char>& anOutBuffer, bool aAllowRedirections)
	{
		std::string filePath = aAllowRedirections ? Core::FilesRedirectionModule::GetInstance()->RedirectFilePath(aFilePath) : aFilePath;
		std::ifstream file(filePath, std::ios::ate | std::ios::binary);
		if (!file.is_open())
		{
			std::cout << "Failed to read the file %s" << filePath << std::endl;
			return false;
		}
		std::streamsize fileSize = file.tellg();
		anOutBuffer.resize(fileSize);
		file.seekg(0);
		file.read(anOutBuffer.data(), fileSize);
		file.close();
		return true;
	}

	bool ReadAsString(const char* aFilePath, std::string& anOutString, bool aAllowRedirections)
	{
		std::string filePath = aAllowRedirections ? Core::FilesRedirectionModule::GetInstance()->RedirectFilePath(aFilePath) : aFilePath;
		std::ifstream file(filePath, std::ios::ate);
		if (!file.is_open())
		{
			std::cout << "Failed to read the file %s" << filePath << std::endl;
			return false;
		}
		std::streamsize fileSize = file.tellg();
		anOutString.resize(fileSize);
		file.seekg(0);
		file.read(anOutString.data(), fileSize);
		file.close();
		return true;
	}

	std::string RedirectFilePath(const char* aFilePath)
	{
		return Core::FilesRedirectionModule::GetInstance()->RedirectFilePath(aFilePath);
	}
}

namespace Core
{
	void FilesRedirectionModule::AddFilesRedirection(const char* aRedirectionPath)
	{
		myRedirections.push_back(aRedirectionPath);
	}

	std::string FilesRedirectionModule::RedirectFilePath(const char* aFilePath)
	{
		if (FileHelpers::FileExists(aFilePath))
			return aFilePath;

		for (const std::string& redirection : myRedirections)
		{
			std::string filePath = redirection + aFilePath;
			if (FileHelpers::FileExists(filePath.c_str()))
				return filePath;
		}

		// No successful path found
		std::cout << "Failed to redirect the file %s" << aFilePath << std::endl;
		return aFilePath;
	}
}
