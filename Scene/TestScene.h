#pragma once
#include "SceneBase.h"

class AimCursor;

class TestScene : public SceneBase
{
	AimCursor* pAimCursor_[2];

public:
	TestScene(GameObject* parent);
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Release() override;
	void CommonUIDraw() override;
	void IndividualUIDraw() override;

	void SetAimCursor(int index, AimCursor* point);
	AimCursor* GetAimCursor(int index);

};