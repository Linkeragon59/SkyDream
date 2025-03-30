#pragma once

#include "Diffraction_Data.h"

#include "Core_Module.h"
#include "Render_RenderModule.h"

namespace Diffraction
{
	class DiffractionRenderer;
	class LightSpectrum;

	class DiffractionModule : public Core::Module
	{
		DECLARE_CORE_MODULE(DiffractionModule, "Diffraction", { "Render" })

	public:
		Render::RenderTargetHandle GetRenderTarget() const;
		Render::RenderTargetHandle GetSecondarySourcePointTarget() const;
		Render::RenderTargetHandle GetResultPointTarget() const;

		enum class Experiment
		{
			None,
			PointLaser,
			SlitsInterference,
			SquareHoleDiffraction,
			RoundHolesInterference,
		};
		void StartExperiment(Experiment anExperiment);
		void EnablePointTargets(bool anEnable);

		void SetDiffractionData(const BaseDiffractionData& someData) { myDiffractionData = someData; }
		void SetPointLaserData(const PointLaserData& someData) { myPointLaserData = someData; }
		void SetSlitsInterferenceData(const SlitsInterferenceData& someData) { mySlitsInterferenceData = someData; }
		void SetSquareHoleDiffractionData(const SquareHoleDiffractionData& someData) { mySquareHoleDiffractionData = someData; }
		void SetRoundHolesInterferenceData(const RoundHolesInterferenceData& someData) { myRoundHolesInterferenceData = someData; }

		void SetSecondarySourcePointDiffractionData(const BaseDiffractionData& someData) { mySecondarySourcePointDiffractionData = someData; }
		void SetResultPointDiffractionData(const BaseDiffractionData& someData) { myResultPointDiffractionData = someData; }
		void SetSecondarySourceAndResultPointLaserData(const PointLaserData& someData) { mySecondarySourceAndResultPointLaserData = someData; }

		void GetLightSpectrumColor(float aWaveLength, float aMinWaveLength, float aMaxWaveLength, glm::u8vec4& anOutColor);

	protected:
		void OnInitialize() override;
		void OnFinalize() override;

		void OnUpdate(Core::Module::UpdateType aType) override;

	private:
		BaseDiffractionData myDiffractionData = {};
		PointLaserData myPointLaserData = {};
		SlitsInterferenceData mySlitsInterferenceData = {};
		SquareHoleDiffractionData mySquareHoleDiffractionData = {};
		RoundHolesInterferenceData myRoundHolesInterferenceData = {};

		Experiment myCurrentExperiment = Experiment::None;
		DiffractionRenderer* myRenderer = nullptr;

		BaseDiffractionData mySecondarySourcePointDiffractionData = {};
		BaseDiffractionData myResultPointDiffractionData = {};
		PointLaserData mySecondarySourceAndResultPointLaserData = {};

		DiffractionRenderer* mySecondarySourcePointRenderer = nullptr;
		DiffractionRenderer* myResultPointRenderer = nullptr;

		LightSpectrum* myLightSpectrum = nullptr;
	};
}
