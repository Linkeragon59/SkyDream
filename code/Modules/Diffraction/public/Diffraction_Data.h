#pragma once

namespace Diffraction
{
	struct BaseDiffractionData
	{
		float xrange; // cm
		float yrange; // cm
		float screenDistance; // m

		float luminosity;
		float waveLength; // nm
		float minVisibleWaveLength; // nm
		float maxVisibleWaveLength; // nm
	};

	struct PointLaserData
	{
		float attenuation; // cm
	};

	struct SlitsInterferenceData
	{
		float slitsCount;
		float slitsWidth; // um
		float slitsSpacing; // um
		float attenuation; // cm
	};

	struct SquareHoleDiffractionData
	{
		float holeWidth; // um
		float holeHeight; // um
	};

	struct RoundHolesInterferenceData
	{
		float holesCount;
		float holesDiameter; // um
		float holesSpacing; // um
	};
}
