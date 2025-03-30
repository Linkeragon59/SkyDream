#pragma once

#include "rapidjson/document.h"

#include "Core_InputModule.h"

struct GLFWwindow;

struct TangiblePreset
{
	std::string myName;
	float myRadius = 1.f;
	float myAngle = 120.f;
};

struct Tangible
{
	Tangible(const TangiblePreset* aPreset, uint64 aTouch1, uint64 aTouch2, uint64 aTouch3)
		: myPreset(aPreset)
	{
		myTouches = { aTouch1 , aTouch2, aTouch3 };
	}

	const TangiblePreset* myPreset;
	std::array<uint64, 3> myTouches;

	glm::vec2 myPosition = { 0.f, 0.f };
	float myOrientation = 0.f;
	float myRadius = 1.f;
	float myAngle = 120.f;
	float myScore = 0.f;
	bool myIsPending = false;
};
typedef std::vector<Tangible> Tangibles;

struct TangibleZone
{
	glm::vec2 myTopLeft = { 0.f, 0.f };
	glm::vec2 myBottomRight = { 1.f, 1.f };
};

class TangiblesManager
{
public:
	TangiblesManager();
	~TangiblesManager();

	void OnTouch(uint64 aFingerId, double aXPos, double aYPos, bool anUp);

	void Update();
	void ShowInfo(bool* anOpen);

	const Tangibles& GetTangibles(const char* aTangibleID) const;
	const Tangible* GetTangible(const char* aTangibleID) const;
	const TangibleZone* GetZone(const char* aZoneID) const;

private:
	bool LoadConfig(const char* aPresetConfigPath);
	bool LoadPresets(const rapidjson::Document& aDoc);
	bool LoadPreset(const rapidjson::Value& aPreset);
	bool LoadZones(const rapidjson::Document& aDoc);
	bool LoadZone(const rapidjson::Value& aZone);

	bool ComputeTouchesSquaredDistance(uint64 aTouch1, uint64 aTouch2, float& anOutDistance) const;
	bool UpdateTangible(Tangible& anInOutTangible) const;

	std::vector<TangiblePreset> myPresets;

	std::map<uint64, glm::vec2> myTouches;

	std::set<uint64> myOpenTouches;
	std::map<std::string, Tangibles> myTangibles;

	std::map<std::string, TangibleZone> myZones;

#if ALLOW_FAKE_TOUCHES
	bool myDebugDisplayPotentialPresets = false;
	void DebugDisplayPotentialPresets() const;

	struct FakeTangible
	{
		const TangiblePreset* myPreset;
		std::array<uint64, 3> myTouches;

		glm::vec2 myPosition = { 0.f, 0.f };
		float myOrientation = 0.f;

		void GetTouchPositions(glm::vec2& aPoint1, glm::vec2& aPoint2, glm::vec2& aPoint3);
	};
	std::vector<FakeTangible> myFakeTangibles;
#endif
};
