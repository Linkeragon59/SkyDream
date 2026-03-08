#pragma once

namespace Render
{
	struct TextureParams
	{
		std::string myPath;
		bool mySamplerRepeat = true;
	};

	struct RenderTargetParams
	{
		uint myWidth = 0;
		uint myHeight = 0;
		bool mySamplerRepeat = true;
	};
}