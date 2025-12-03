#pragma once

#include "Core_Module.h"

namespace FileHelpers
{
	bool FileExists(const char* aFilePath);
	bool ReadAsBuffer(const char* aFilePath, std::vector<char>& anOutBuffer, bool aAllowRedirections = true);
	bool ReadAsString(const char* aFilePath, std::string& anOutString, bool aAllowRedirections = true);
	std::string RedirectFilePath(const char* aFilePath);
}

namespace Core
{
	class FilesRedirectionModule : public Core::Module
	{
		DECLARE_CORE_MODULE(FilesRedirectionModule, "FilesRedirection")

	public:
		void AddFilesRedirection(const char* aRedirectionPath);
		std::string RedirectFilePath(const char* aFilePath);

	private:
		std::vector<std::string> myRedirections;
	};
}
