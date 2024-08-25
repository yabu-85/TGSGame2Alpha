#pragma once
#include "SceneBase.h"

class AimCursor;
class Player;

class PlayScene : public SceneBase
{
	int time_;					//Œo‰ßŽžŠÔ

	Player* pPlayer_[2];
	AimCursor* pAimCursor_[2];

public:
	PlayScene(GameObject* parent);
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Release() override;
	void CommonUIDraw() override;
	void IndividualUIDraw(int index) override;

	void SetAimCursor(int index, AimCursor* point) { pAimCursor_[index] = point; }
	AimCursor* GetAimCursor(int index) { return pAimCursor_[index]; }

};