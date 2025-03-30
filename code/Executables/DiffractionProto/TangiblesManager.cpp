#include "TangiblesManager.h"

#include "DiffractionProtoModule.h"

#include "Core_WindowModule.h"
#include "Core_FileHelpers.h"

#include "imgui.h"

namespace TangiblesManager_Priv
{
	// TODO : Use physical sizes

	const char* locTangiblePresetsKey = "TangiblePresets";
	const char* locTangiblePresetNameKey = "Name";
	const char* locTangiblePresetRadiusKey = "Radius";
	const char* locTangiblePresetAngleKey = "Angle";

	const char* locTangibleZonesKey = "TangibleZones";
	const char* locTangibleZoneNameKey = "Name";
	const char* locTangibleZoneLeftKey = "Left";
	const char* locTangibleZoneRightKey = "Right";
	const char* locTangibleZoneTopKey = "Top";
	const char* locTangibleZoneBottomKey = "Bottom";

	float locNeighborsThreshold = 1.1f;
	float locRadiusVariance = 100.f;
	float locAngleVariance = 25.f;
	float locScoreThreshold = 0.7f;

	float locRadiusDoubleSquaredVariance = 2.f * locRadiusVariance * locRadiusVariance;
	float locAngleDoubleSquaredVariance = 2.f * locAngleVariance * locAngleVariance;

	bool ComputeCircle(const std::vector<glm::vec2>& somePoints, glm::vec2& anOutCenter, float& anOutRadius)
	{
		glm::mat3x3 matrix;
		for (uint i = 0; i < 3; ++i)
		{
			matrix[i][0] = somePoints[i].x;
			matrix[i][1] = somePoints[i].y;
			matrix[i][2] = 1.f;
		}
		float m11 = glm::determinant(matrix);
		if (abs(m11) <= FLT_EPSILON)
			return false;

		for (uint i = 0; i < 3; ++i)
		{
			matrix[i][0] = somePoints[i].x * somePoints[i].x + somePoints[i].y * somePoints[i].y;
		}
		float m12 = glm::determinant(matrix);

		for (uint i = 0; i < 3; ++i)
		{
			matrix[i][0] = somePoints[i].x * somePoints[i].x + somePoints[i].y * somePoints[i].y;
			matrix[i][1] = somePoints[i].x;
		}
		float m13 = glm::determinant(matrix);

		for (uint i = 0; i < 3; ++i)
		{
			matrix[i][0] = somePoints[i].x * somePoints[i].x + somePoints[i].y * somePoints[i].y;
			matrix[i][2] = somePoints[i].y;
		}
		float m14 = glm::determinant(matrix);

		anOutCenter.x = 0.5f * m12 / m11;
		anOutCenter.y = -0.5f * m13 / m11;
		anOutRadius = std::sqrtf(anOutCenter.x * anOutCenter.x + anOutCenter.y * anOutCenter.y + m14 / m11);
		return true;
	}

	float ComputeAngle(const glm::vec2& aPoint1, const glm::vec2& aPoint2, const glm::vec2& aPoint3)
	{
		glm::vec2 vec1 = aPoint2 - aPoint1;
		glm::vec2 vec2 = aPoint3 - aPoint1;
		return glm::degrees(std::acosf((glm::dot(vec1, vec2)) / (glm::length(vec1) * glm::length(vec2))));
	}

	float ComputeOrientation(const glm::vec2& aPoint1, const glm::vec2& aPoint2, const glm::vec2& aPoint3)
	{
		glm::vec2 vec1 = aPoint2 - aPoint1;
		glm::vec2 vec2 = aPoint3 - aPoint1;
		return glm::degrees(std::atan2f(vec1.x * vec2.y - vec1.y * vec2.x, vec1.x * vec2.x - vec1.y * vec2.y));
	}
}

TangiblesManager::TangiblesManager()
{
	LoadConfig("Executables/DiffractionProto/Configs/Tangibles.json");
}

TangiblesManager::~TangiblesManager()
{
}

void TangiblesManager::OnTouch(uint64 aFingerId, double aXPos, double aYPos, bool anUp)
{
	if (anUp)
	{
		myTouches.erase(aFingerId);
		myOpenTouches.erase(aFingerId);
		return;
	}
	if (myTouches.find(aFingerId) == myTouches.end())
		myOpenTouches.insert(aFingerId);
	myTouches[aFingerId] = { (float)aXPos , (float)aYPos };
}

void TangiblesManager::Update()
{
	// First check the current Tangibles and update those that still exist
	for (auto it = myTangibles.begin(); it != myTangibles.end();)
	{
		Tangibles& tangibles = it->second;
		for (auto it2 = tangibles.begin(); it2 != tangibles.end();)
		{
			Tangible& tangible = *it2;
			if (!UpdateTangible(tangible))
			{
				for (uint64 touchId : tangible.myTouches)
					if (myTouches.find(touchId) != myTouches.end())
						myOpenTouches.insert(touchId);

				if (!tangible.myIsPending)
				{
					it2 = tangibles.erase(it2);
					continue;
				}
			}
			++it2;
		}
		if (tangibles.empty())
		{
			it = myTangibles.erase(it);
			continue;
		}
		++it;
	}

	// Then check for new Tangibles
	Tangibles tangibleCandidates;

	std::set<uint64>::iterator it = myOpenTouches.begin();
	while (it != myOpenTouches.end())
	{
		uint64 touchId = *it;
		it++;

		for (const TangiblePreset& preset : myPresets)
		{
			float squaredLimitDistance = std::powf(2.f * preset.myRadius, 2.f) * TangiblesManager_Priv::locNeighborsThreshold;

			std::set<uint64> neighbors;
			for (std::set<uint64>::iterator it2 = it; it2 != myOpenTouches.end(); ++it2)
			{
				float squaredDistance;
				if (ComputeTouchesSquaredDistance(touchId, *it2, squaredDistance) && squaredDistance <= squaredLimitDistance)
					neighbors.insert(*it2);
			}

			std::set<uint64>::iterator it2 = neighbors.begin();
			while (it2 != neighbors.end())
			{
				uint64 touchId2 = *it2;
				it2++;

				for (std::set<uint64>::iterator it3 = it2; it3 != neighbors.end(); ++it3)
				{
					uint64 touchId3 = *it3;

					Tangible candidateTangible(&preset, touchId, touchId2, touchId3);
					if (!UpdateTangible(candidateTangible))
						continue;

					tangibleCandidates.push_back(candidateTangible);
				}
			}
		}
	}

	if (!tangibleCandidates.empty())
	{
		std::sort(tangibleCandidates.begin(), tangibleCandidates.end(), [](const Tangible& aTangible1, const Tangible& aTangible2) {
			return aTangible1.myScore > aTangible2.myScore;
		});

		for (const Tangible& tangibleCandidate : tangibleCandidates)
		{
			if (std::any_of(tangibleCandidate.myTouches.begin(), tangibleCandidate.myTouches.end(), [this](uint64 aTouchId) {
				return myOpenTouches.find(aTouchId) == myOpenTouches.end();
			}))
				continue;

			myTangibles[tangibleCandidate.myPreset->myName].push_back(tangibleCandidate);
			for (uint64 touchId : tangibleCandidate.myTouches)
			{
				myOpenTouches.erase(touchId);
				
				// Remove any pending tangible that is using the touchId we are closing
				for (auto tangibleIter = myTangibles.begin(); tangibleIter != myTangibles.end();)
				{
					Tangibles& tangibles = tangibleIter->second;
					for (auto tangibleIter2 = tangibles.begin(); tangibleIter2 != tangibles.end();)
					{
						Tangible& tangible = *tangibleIter2;
						if (tangible.myIsPending && std::find(tangible.myTouches.begin(), tangible.myTouches.end(), touchId) != tangible.myTouches.end())
						{
							tangibleIter2 = tangibles.erase(tangibleIter2);
							continue;
						}
						++tangibleIter2;
					}
					if (tangibles.empty())
					{
						tangibleIter = myTangibles.erase(tangibleIter);
						continue;
					}
					++tangibleIter;
				}
			}
		}
	}
}

void TangiblesManager::ShowInfo(bool* anOpen)
{
#if ALLOW_FAKE_TOUCHES
	Core::InputModule* inputModule = Core::InputModule::GetInstance();
	GLFWwindow* window = DiffractionProtoModule::GetInstance()->GetSecondaryWindow();
	auto props = Core::WindowModule::GetInstance()->GetWindowProperties(window);
#endif

	if (ImGui::Begin(Loc::GetLocString("TangiblesInfoWindow"), anOpen, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("%d %s", myPresets.size(), Loc::GetLocString("TangiblesPresets"));
		for (const TangiblePreset& preset : myPresets)
		{
			ImGui::Text("%s : %.2f, %.2f", Loc::GetLocString(preset.myName.c_str()), preset.myRadius, preset.myAngle);
#if ALLOW_FAKE_TOUCHES
			ImGui::SameLine();
			ImGui::PushID(&preset);
			if (ImGui::Button(Loc::GetLocString("AddFake")))
			{
				FakeTangible newFakeTangible;
				newFakeTangible.myPreset = &preset;
				newFakeTangible.myPosition = { props->myWidth / 2.0, props->myHeight / 2.0 };
				newFakeTangible.myOrientation = -90.f;

				glm::vec2 point1, point2, point3;
				newFakeTangible.GetTouchPositions(point1, point2, point3);

				newFakeTangible.myTouches[0] = inputModule->FakeTouchBegin(window, point1.x, point1.y);
				newFakeTangible.myTouches[1] = inputModule->FakeTouchBegin(window, point2.x, point2.y);
				newFakeTangible.myTouches[2] = inputModule->FakeTouchBegin(window, point3.x, point3.y);

				myFakeTangibles.push_back(newFakeTangible);
			}
			ImGui::PopID();
#endif
		}

		ImGui::Separator();
		ImGui::Text("%d %s", myTangibles.size(), Loc::GetLocString("TangiblesDetected"));
		for (auto it = myTangibles.begin(); it != myTangibles.end(); ++it)
		{
			for (const Tangible& tangible : it->second)
			{
				ImGui::Text(Loc::GetLocString(tangible.myPreset->myName.c_str()));
				ImGui::Indent();
				ImGui::Text("%s: (%.0f, %.0f), %s: %.2f, %s: %.2f",
					Loc::GetLocString("TangiblePosition"),
					tangible.myPosition.x, tangible.myPosition.y,
					Loc::GetLocString("TangibleOrientation"),
					tangible.myOrientation,
					Loc::GetLocString("TangibleScore"),
					tangible.myScore);
				ImGui::Text("%s: %.2f, %s:%.2f", Loc::GetLocString("TangibleRadius"), tangible.myRadius, Loc::GetLocString("TangibleAngle"), tangible.myAngle);
				ImGui::Text("%s: %s:%.2f, %s:%.2f", Loc::GetLocString("TangiblePreset"), Loc::GetLocString("TangibleRadius"), tangible.myPreset->myRadius, Loc::GetLocString("TangibleAngle"), tangible.myPreset->myAngle);
				ImGui::Unindent();
			}
		}

#if ALLOW_FAKE_TOUCHES
		ImGui::Separator();
		ImGui::Text("%d %s", myFakeTangibles.size(), Loc::GetLocString("FakeTangibles"));
		for (auto it = myFakeTangibles.begin(); it != myFakeTangibles.end();)
		{
			FakeTangible& fakeTangible = *it;
			ImGui::PushID(&fakeTangible);

			ImGui::Text(Loc::GetLocString(fakeTangible.myPreset->myName.c_str()));
			ImGui::Indent();
			ImGui::SliderFloat(Loc::GetLocString("TangibleXPos"), &fakeTangible.myPosition.x, 0.f, static_cast<float>(props->myWidth));
			ImGui::SliderFloat(Loc::GetLocString("TangibleYPos"), &fakeTangible.myPosition.y, 0.f, static_cast<float>(props->myHeight));
			ImGui::SliderFloat(Loc::GetLocString("TangibleOrientation"), &fakeTangible.myOrientation, -180.f, 180.f);

			glm::vec2 point1, point2, point3;
			fakeTangible.GetTouchPositions(point1, point2, point3);

			inputModule->FakeTouchMove(window, fakeTangible.myTouches[0], point1.x, point1.y);
			inputModule->FakeTouchMove(window, fakeTangible.myTouches[1], point2.x, point2.y);
			inputModule->FakeTouchMove(window, fakeTangible.myTouches[2], point3.x, point3.y);

			if (ImGui::Button(Loc::GetLocString("RemoveTangible")))
			{
				inputModule->FakeTouchEnd(window, fakeTangible.myTouches[0]);
				inputModule->FakeTouchEnd(window, fakeTangible.myTouches[1]);
				inputModule->FakeTouchEnd(window, fakeTangible.myTouches[2]);

				it = myFakeTangibles.erase(it);
			}
			else
			{
				++it;
			}

			ImGui::Unindent();
			ImGui::PopID();
		}

		ImGui::Separator();
		ImGui::Text(Loc::GetLocString("CustomizeTolerances"));
		ImGui::Indent();
		//ImGui::SliderFloat("Neighbor Threshold", &TangiblesManager_Priv::locNeighborsThreshold, 1.f, 1.25f);
		ImGui::SliderFloat(Loc::GetLocString("RadiusVariance"), &TangiblesManager_Priv::locRadiusVariance, 0.f, 200.f);
		ImGui::SliderFloat(Loc::GetLocString("AngleVariance"), &TangiblesManager_Priv::locAngleVariance, 0.f, 50.f);
		ImGui::SliderFloat(Loc::GetLocString("ScoreThreshold"), &TangiblesManager_Priv::locScoreThreshold, 0.f, 1.f);
		ImGui::Unindent();

		TangiblesManager_Priv::locRadiusDoubleSquaredVariance = 2.f * TangiblesManager_Priv::locRadiusVariance * TangiblesManager_Priv::locRadiusVariance;
		TangiblesManager_Priv::locAngleDoubleSquaredVariance = 2.f * TangiblesManager_Priv::locAngleVariance * TangiblesManager_Priv::locAngleVariance;

		ImGui::Separator();
		ImGui::Checkbox(Loc::GetLocString("DisplayMatchingPresets"), &myDebugDisplayPotentialPresets);
		if (myDebugDisplayPotentialPresets)
			DebugDisplayPotentialPresets();
#endif
	}
	ImGui::End();
}

const Tangibles& TangiblesManager::GetTangibles(const char* aTangibleID) const
{
	auto it = myTangibles.find(aTangibleID);
	if (it != myTangibles.end())
		return it->second;

	static Tangibles empty;
	return empty;
}

const Tangible* TangiblesManager::GetTangible(const char* aTangibleID) const
{
	auto it = myTangibles.find(aTangibleID);
	if (it != myTangibles.end() && it->second.size() > 0)
		return &it->second[0];
	return nullptr;
}

const TangibleZone* TangiblesManager::GetZone(const char* aZoneID) const
{
	auto it = myZones.find(aZoneID);
	if (it != myZones.end())
		return &it->second;
	return nullptr;
}

bool TangiblesManager::LoadConfig(const char* aPresetConfigPath)
{
	std::string tangiblesStr;
	if (!FileHelpers::ReadAsString(aPresetConfigPath, tangiblesStr))
		return false;

	rapidjson::Document tangiblesDoc;
	tangiblesDoc.Parse(tangiblesStr.c_str());

	if (!LoadPresets(tangiblesDoc))
		return false;

	if (!LoadZones(tangiblesDoc))
		return false;

	return true;
}

bool TangiblesManager::LoadPresets(const rapidjson::Document& aDoc)
{
	if (!aDoc.HasMember(TangiblesManager_Priv::locTangiblePresetsKey) || !aDoc[TangiblesManager_Priv::locTangiblePresetsKey].IsArray())
		return false;

	for (auto& tangiblePreset : aDoc[TangiblesManager_Priv::locTangiblePresetsKey].GetArray())
		LoadPreset(tangiblePreset);

	return true;
}

bool TangiblesManager::LoadPreset(const rapidjson::Value& aPreset)
{
	if (!aPreset.HasMember(TangiblesManager_Priv::locTangiblePresetNameKey) || !aPreset[TangiblesManager_Priv::locTangiblePresetNameKey].IsString())
		return false;

	if (!aPreset.HasMember(TangiblesManager_Priv::locTangiblePresetRadiusKey) || !aPreset[TangiblesManager_Priv::locTangiblePresetRadiusKey].IsNumber())
		return false;

	if (!aPreset.HasMember(TangiblesManager_Priv::locTangiblePresetAngleKey) || !aPreset[TangiblesManager_Priv::locTangiblePresetAngleKey].IsNumber())
		return false;

	TangiblePreset& preset = myPresets.emplace_back();
	preset.myName = aPreset[TangiblesManager_Priv::locTangiblePresetNameKey].GetString();
	preset.myRadius = aPreset[TangiblesManager_Priv::locTangiblePresetRadiusKey].GetFloat();
	preset.myAngle = aPreset[TangiblesManager_Priv::locTangiblePresetAngleKey].GetFloat();
	return true;
}

bool TangiblesManager::LoadZones(const rapidjson::Document& aDoc)
{
	if (!aDoc.HasMember(TangiblesManager_Priv::locTangibleZonesKey) || !aDoc[TangiblesManager_Priv::locTangibleZonesKey].IsArray())
		return false;

	for (auto& tangibleZone : aDoc[TangiblesManager_Priv::locTangibleZonesKey].GetArray())
		LoadZone(tangibleZone);

	return true;
}

bool TangiblesManager::LoadZone(const rapidjson::Value& aZone)
{
	if (!aZone.HasMember(TangiblesManager_Priv::locTangibleZoneNameKey) || !aZone[TangiblesManager_Priv::locTangibleZoneNameKey].IsString())
		return false;

	if (!aZone.HasMember(TangiblesManager_Priv::locTangibleZoneLeftKey) || !aZone[TangiblesManager_Priv::locTangibleZoneLeftKey].IsNumber())
		return false;

	if (!aZone.HasMember(TangiblesManager_Priv::locTangibleZoneRightKey) || !aZone[TangiblesManager_Priv::locTangibleZoneRightKey].IsNumber())
		return false;

	if (!aZone.HasMember(TangiblesManager_Priv::locTangibleZoneTopKey) || !aZone[TangiblesManager_Priv::locTangibleZoneTopKey].IsNumber())
		return false;

	if (!aZone.HasMember(TangiblesManager_Priv::locTangibleZoneBottomKey) || !aZone[TangiblesManager_Priv::locTangibleZoneBottomKey].IsNumber())
		return false;

	TangibleZone& zone = myZones[aZone[TangiblesManager_Priv::locTangibleZoneNameKey].GetString()];
	zone.myTopLeft.x = aZone[TangiblesManager_Priv::locTangibleZoneLeftKey].GetFloat();
	zone.myBottomRight.x = aZone[TangiblesManager_Priv::locTangibleZoneRightKey].GetFloat();
	zone.myTopLeft.y = aZone[TangiblesManager_Priv::locTangibleZoneTopKey].GetFloat();
	zone.myBottomRight.y = aZone[TangiblesManager_Priv::locTangibleZoneBottomKey].GetFloat();
	return true;
}

bool TangiblesManager::ComputeTouchesSquaredDistance(uint64 aTouch1, uint64 aTouch2, float& anOutDistance) const
{
	auto touch1 = myTouches.find(aTouch1);
	if (touch1 == myTouches.end())
		return false;
	auto touch2 = myTouches.find(aTouch2);
	if (touch2 == myTouches.end())
		return false;

	anOutDistance = glm::distance2(touch1->second, touch2->second);
	return true;
}

bool TangiblesManager::UpdateTangible(Tangible& anInOutTangible) const
{
	std::vector<glm::vec2> points;
	for (uint64 touchId : anInOutTangible.myTouches)
	{
		auto touch = myTouches.find(touchId);
		if (touch != myTouches.end())
		{
			points.push_back(touch->second);
		}
	}

	if (points.size() != 3)
	{
		anInOutTangible.myIsPending = (points.size() != 0);
		return false;
	}

	// First check we can compute a circle that passes by the 3 points
	if (!TangiblesManager_Priv::ComputeCircle(points, anInOutTangible.myPosition, anInOutTangible.myRadius))
		return false;

	// Then check the radius against the preset
	anInOutTangible.myScore = std::expf(-std::powf(anInOutTangible.myRadius - anInOutTangible.myPreset->myRadius, 2.f) / TangiblesManager_Priv::locRadiusDoubleSquaredVariance);
	if (anInOutTangible.myScore < TangiblesManager_Priv::locScoreThreshold)
		return false;

	// If radius is fine, then find the angle (two angles should be pretty close, use the third one (the one the further away from the average)
	std::array<float, 3> angles;
	uint maxAnglePoint = 0;
	float maxAngle = 0.f;
	float angleAverage = 0.f;
	for (uint i = 0; i < 3; ++i)
	{
		angles[i] = TangiblesManager_Priv::ComputeAngle(anInOutTangible.myPosition, points[(i + 1) % 3], points[(i + 2) % 3]);
		angleAverage += angles[i];
		if (angles[i] > maxAngle)
		{
			maxAngle = angles[i];
			maxAnglePoint = i;
		}
	}
	if (angleAverage < 359.f)
	{
		// Inverse the max angle
		angles[maxAnglePoint] = 360.f - angles[maxAnglePoint];
	}
	angleAverage /= 3.f;
	uint rootPoint = 0;
	float maxDiff = 0.f;
	for (uint i = 0; i < 3; ++i)
	{
		float angleDiff = std::abs(angles[i] - angleAverage);
		if (angleDiff > maxDiff)
		{
			rootPoint = i;
			maxDiff = angleDiff;
			anInOutTangible.myAngle = angles[i];
		}
	}

	// Then check that angle against the preset
	anInOutTangible.myScore *= std::expf(-std::powf(anInOutTangible.myAngle - anInOutTangible.myPreset->myAngle, 2.f) / TangiblesManager_Priv::locAngleDoubleSquaredVariance);
	if (anInOutTangible.myScore < TangiblesManager_Priv::locScoreThreshold)
		return false;

	// Finally compute the orientation
	anInOutTangible.myOrientation = TangiblesManager_Priv::ComputeOrientation(anInOutTangible.myPosition, anInOutTangible.myPosition + glm::vec2(0.f, 1.f), points[rootPoint]);
	return true;
}

#if ALLOW_FAKE_TOUCHES
void TangiblesManager::DebugDisplayPotentialPresets() const
{
	if (myTouches.size() != 3)
	{
		ImGui::Text(Loc::GetLocString("CurrentlyNotDetecting3Touches"));
		return;
	}

	std::vector<glm::vec2> points;
	for (const auto& touch : myTouches)
	{
		points.push_back(touch.second);
	}

	glm::vec2 position;
	float radius;
	if (!TangiblesManager_Priv::ComputeCircle(points, position, radius))
		return;

	std::array<float, 3> angles;
	uint maxAnglePoint = 0;
	float maxAngle = 0.f;
	float angleAverage = 0.f;
	for (uint i = 0; i < 3; ++i)
	{
		angles[i] = TangiblesManager_Priv::ComputeAngle(position, points[(i + 1) % 3], points[(i + 2) % 3]);
		angleAverage += angles[i];
		if (angles[i] > maxAngle)
		{
			maxAngle = angles[i];
			maxAnglePoint = i;
		}
	}
	if (angleAverage < 359.f)
	{
		// Inverse the max angle
		angles[maxAnglePoint] = 360.f - angles[maxAnglePoint];
	}
	angleAverage /= 3.f;
	uint rootPoint = 0;
	float maxDiff = 0.f;
	for (uint i = 0; i < 3; ++i)
	{
		float angleDiff = std::abs(angles[i] - angleAverage);
		if (angleDiff > maxDiff)
		{
			rootPoint = i;
			maxDiff = angleDiff;
		}
	}
	ImGui::Text("%.2f, %.2f", radius, angles[rootPoint]);
}
#endif

#if ALLOW_FAKE_TOUCHES
void TangiblesManager::FakeTangible::GetTouchPositions(glm::vec2& aPoint1, glm::vec2& aPoint2, glm::vec2& aPoint3)
{
	float anglePoint2 = myOrientation + 180 + myPreset->myAngle / 2.f;
	float anglePoint3 = myOrientation + 180 - myPreset->myAngle / 2.f;
	aPoint1 = myPosition - glm::vec2(std::sinf(glm::radians(myOrientation)) * myPreset->myRadius, std::cosf(glm::radians(myOrientation)) * myPreset->myRadius);
	aPoint2 = myPosition - glm::vec2(std::sinf(glm::radians(anglePoint2)) * myPreset->myRadius, std::cosf(glm::radians(anglePoint2)) * myPreset->myRadius);
	aPoint3 = myPosition - glm::vec2(std::sinf(glm::radians(anglePoint3)) * myPreset->myRadius, std::cosf(glm::radians(anglePoint3)) * myPreset->myRadius);
}
#endif
