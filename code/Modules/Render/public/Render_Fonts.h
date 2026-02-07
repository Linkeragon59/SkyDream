#pragma once

struct ImFont;

namespace Render
{
	enum class FontType
	{
		Regular,
		Bold,
		Italic,
		Large,
		Title,
		Count
	};

	struct FontDesc
	{
		void Clear() { myFont = nullptr; mySize = 0.f; }
		ImFont* myFont = nullptr;
		float mySize = 0.f;
	};

	struct FontMap
	{
		void Clear()
		{
			for (FontDesc& font : myFonts)
				font.Clear();
		}
		void SetFont(FontType aFontType, ImFont* aFont, float aFontSize) { myFonts[(size_t)aFontType] = { aFont, aFontSize }; }
		const FontDesc& GetFont(FontType aFontType) const { return myFonts[(size_t)aFontType]; }
	private:
		std::array<FontDesc, (size_t)FontType::Count> myFonts;
	};
}
