#include "Diffraction_Module.h"

#include "Diffraction_LightSpectrum.h"
#include "Diffraction_Renderer.h"

#include "Render_RenderCore.h"

namespace Diffraction
{
	Render::RenderTargetHandle DiffractionModule::GetRenderTarget() const
	{
		return myRenderer->GetRenderTarget();
	}

	Render::RenderTargetHandle DiffractionModule::GetSecondarySourcePointTarget() const
	{
		return mySecondarySourcePointRenderer->GetRenderTarget();
	}

	Render::RenderTargetHandle DiffractionModule::GetResultPointTarget() const
	{
		return myResultPointRenderer->GetRenderTarget();
	}

	void DiffractionModule::StartExperiment(Experiment anExperiment)
	{
		if (anExperiment == myCurrentExperiment)
			return;

		myCurrentExperiment = anExperiment;

		if (myRenderer)
		{
			RenderCore::GetInstance()->DestroyRenderTarget(myRenderer->GetRenderTarget());
		}

		if (myCurrentExperiment == Experiment::None)
			return;

		RenderTarget* renderTarget = RenderCore::GetInstance()->CreateRenderTarget(1920, 1080);
		switch (myCurrentExperiment)
		{
		case Experiment::PointLaser:
			myRenderer = new DiffractionRenderer(*renderTarget, "Modules/Diffraction/Shaders/PointLaser_frag.spv", sizeof(SlitsInterferenceData));
			break;
		case Experiment::SlitsInterference:
			myRenderer = new DiffractionRenderer(*renderTarget, "Modules/Diffraction/Shaders/SlitsInterference_frag.spv", sizeof(SlitsInterferenceData));
			break;
		case Experiment::SquareHoleDiffraction:
			myRenderer = new DiffractionRenderer(*renderTarget, "Modules/Diffraction/Shaders/SquareHoleDiffraction_frag.spv", sizeof(SquareHoleDiffractionData));
			break;
		case Experiment::RoundHolesInterference:
			myRenderer = new DiffractionRenderer(*renderTarget, "Modules/Diffraction/Shaders/RoundHolesInterference_frag.spv", sizeof(RoundHolesInterferenceData));
			break;
		default:
			Assert(false, "Unsupported Experiment");
			RenderCore::GetInstance()->DestroyRenderTarget(renderTarget);
			return;
		}
		renderTarget->AttachRenderer(myRenderer);
	}

	void DiffractionModule::EnablePointTargets(bool anEnable)
	{
		bool isEnabled = mySecondarySourcePointRenderer != nullptr && myResultPointRenderer != nullptr;
		if (anEnable == isEnabled)
			return;

		if (anEnable)
		{
			RenderTarget* secondarySourcePointRenderTarget = RenderCore::GetInstance()->CreateRenderTarget(100, 100);
			mySecondarySourcePointRenderer = new DiffractionRenderer(*secondarySourcePointRenderTarget, "Modules/Diffraction/Shaders/PointLaser_frag.spv", sizeof(SlitsInterferenceData));
			secondarySourcePointRenderTarget->AttachRenderer(mySecondarySourcePointRenderer);
			RenderTarget* resultPointRenderTarget = RenderCore::GetInstance()->CreateRenderTarget(100, 100);
			myResultPointRenderer = new DiffractionRenderer(*resultPointRenderTarget, "Modules/Diffraction/Shaders/PointLaser_frag.spv", sizeof(SlitsInterferenceData));
			resultPointRenderTarget->AttachRenderer(myResultPointRenderer);
		}
		else
		{
			RenderCore::GetInstance()->DestroyRenderTarget(mySecondarySourcePointRenderer->GetRenderTarget());
			RenderCore::GetInstance()->DestroyRenderTarget(myResultPointRenderer->GetRenderTarget());

			mySecondarySourcePointRenderer = nullptr;
			myResultPointRenderer = nullptr;
		}
	}

	void DiffractionModule::GetLightSpectrumColor(float aWaveLength, float aMinWaveLength, float aMaxWaveLength, glm::u8vec4& anOutColor)
	{
		return myLightSpectrum->GetLightSpectrumColor(aWaveLength, aMinWaveLength, aMaxWaveLength, anOutColor);
	}

	void DiffractionModule::OnInitialize()
	{
		myLightSpectrum = new LightSpectrum();
	}

	void DiffractionModule::OnFinalize()
	{
		StartExperiment(Experiment::None);
		EnablePointTargets(false);
		SafeDelete(myLightSpectrum);
	}

	void DiffractionModule::OnUpdate(Core::Module::UpdateType aType)
	{
		if (aType == Core::Module::UpdateType::MainUpdate)
		{
			switch (myCurrentExperiment)
			{
			case Experiment::PointLaser:
				myRenderer->Draw(&myDiffractionData, &myPointLaserData, myLightSpectrum->GetLightSpectrumDescriptor());
				break;
			case Experiment::SlitsInterference:
				myRenderer->Draw(&myDiffractionData, &mySlitsInterferenceData, myLightSpectrum->GetLightSpectrumDescriptor());
				break;
			case Experiment::SquareHoleDiffraction:
				myRenderer->Draw(&myDiffractionData, &mySquareHoleDiffractionData, myLightSpectrum->GetLightSpectrumDescriptor());
				break;
			case Experiment::RoundHolesInterference:
				myRenderer->Draw(&myDiffractionData, &myRoundHolesInterferenceData, myLightSpectrum->GetLightSpectrumDescriptor());
				break;
			default:
				break;
			}

			if (mySecondarySourcePointRenderer)
				mySecondarySourcePointRenderer->Draw(&mySecondarySourcePointDiffractionData, &mySecondarySourceAndResultPointLaserData, myLightSpectrum->GetLightSpectrumDescriptor());
			if (myResultPointRenderer)
				myResultPointRenderer->Draw(&myResultPointDiffractionData, &mySecondarySourceAndResultPointLaserData, myLightSpectrum->GetLightSpectrumDescriptor());
		}
	}
}
