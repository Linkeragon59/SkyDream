#include "Localization_Module.h"

#include <fstream>

namespace Loc
{

	bool LoadLocFile(const char* aFilePath, const char* aFileName)
	{
		return Localization::LocalizationModule::GetInstance()->LoadLocalizationFile(aFilePath, aFileName);
	}

	Lang GetLang()
	{
		return Localization::LocalizationModule::GetInstance()->GetLanguage();
	}

	void SetLang(Lang aLang)
	{
		Localization::LocalizationModule::GetInstance()->SetLanguage(aLang);
	}

	const char* GetLocString(const char* aLocId)
	{
		return Localization::LocalizationModule::GetInstance()->GetLocalizedString(aLocId);
	}

}

namespace Localization
{

	namespace LocalizationModule_Priv
	{
		const char* locEN = ".en";
		const char* locFR = ".fr";
		const char* locMissingString = "???";
		char locIdPrefix = '#';
	}

	void LocalizationModule::OnInitialize()
	{
		LoadLocalizationFile("Frameworks/Localization/", "Core");
	}

	bool LocalizationModule::LoadLocalizationFile(const char* aFilePath, const char* aFileName)
	{
		if (!LoadLocalizationFile(aFilePath, aFileName, Loc::Lang::En))
			return false;
		LoadLocalizationFile(aFilePath, aFileName, Loc::Lang::Fr);
		return true;
	}

	const char* LocalizationModule::GetLocalizedString(const char* aLocalizationId) const
	{
		auto it = myLocalizedStrings.find(aLocalizationId);
		if (it == myLocalizedStrings.end())
			return LocalizationModule_Priv::locMissingString;

		const LocalizedString& localizedString = it->second;
		auto it2 = localizedString.find(myCurrentLanguage);
		if (it2 == localizedString.end())
		{
			it2 = localizedString.find(Loc::Lang::En);
			if (it2 == localizedString.end())
				return LocalizationModule_Priv::locMissingString;
		}

		return it2->second.c_str();
	}

	bool LocalizationModule::LoadLocalizationFile(const char* aFilePath, const char* aFileName, Loc::Lang aLanguage)
	{
		std::string filePath = aFilePath;
		filePath += aFileName;
		switch (aLanguage)
		{
		case Loc::Lang::En:
			filePath += LocalizationModule_Priv::locEN;
			break;
		case Loc::Lang::Fr:
			filePath += LocalizationModule_Priv::locFR;
			break;
		default:
			Assert(false, "Unsupported Language");
			return false;
		}

		std::ifstream file(filePath);
		if (!file.is_open())
			return false;

		std::string line;
		std::string id;
		while (std::getline(file, line))
		{
			if (line.size() == 0)
				continue;

			if (line[0] == LocalizationModule_Priv::locIdPrefix)
			{
				id = line.erase(0, 1);
			}
			else if (!id.empty())
			{
				auto it = myLocalizedStrings.find(id);
				if (it == myLocalizedStrings.end())
				{
					LocalizedString newString;
					newString[aLanguage] = line;
					myLocalizedStrings[id] = newString;
				}
				else
				{
					it->second[aLanguage] = line;
				}
			}
		}

		file.close();
		return true;
	}

}
