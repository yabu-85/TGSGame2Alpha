#pragma once
#include "SceneBase.h"
#include "../Engine/EffekseeLib/EffekseerVFX.h"

class AimCursor;

class TestScene : public SceneBase
{
	AimCursor* pAimCursor_[2];
	std::shared_ptr<EFFEKSEERLIB::EFKTransform> mt;

public:
	TestScene(GameObject* parent);
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Release() override;
	void CommonUIDraw() override;
	void IndividualUIDraw(int index) override;

	void SetAimCursor(int index, AimCursor* point) { pAimCursor_[index] = point; }
	AimCursor* GetAimCursor(int index) { return pAimCursor_[index]; }

};