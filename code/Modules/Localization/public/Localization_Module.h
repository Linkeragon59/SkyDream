#include "Core_Module.h"

// Helpers to shorten the call syntax
namespace Loc
{
	enum class Lang
	{
		En,
		Fr
	};

	bool LoadLocFile(const char* aFilePath, const char* aFileName);
	Lang GetLang();
	void SetLang(Lang aLang);
	const char* GetLocString(const char* aLocId);
}

// TODO : Improve so we only keep one language loaded at a time, also allow to load/unload loc files
namespace Localization
{
	class LocalizationModule : public Core::Module
	{
		DECLARE_CORE_MODULE(LocalizationModule, "Localization")

	protected:
		void OnInitialize() override;

	public:
		bool LoadLocalizationFile(const char* aFilePath, const char* aFileName);
		Loc::Lang GetLanguage() const { return myCurrentLanguage; }
		void SetLanguage(Loc::Lang aLanguage) { myCurrentLanguage = aLanguage; }
		const char* GetLocalizedString(const char* aLocalizationId) const;

	private:
		bool LoadLocalizationFile(const char* aFilePath, const char* aFileName, Loc::Lang aLanguage);

		typedef std::map<Loc::Lang, std::string> LocalizedString;
		typedef std::map<std::string, LocalizedString> LocalizedStrings;
		LocalizedStrings myLocalizedStrings;
		Loc::Lang myCurrentLanguage = Loc::Lang::En;
	};
}