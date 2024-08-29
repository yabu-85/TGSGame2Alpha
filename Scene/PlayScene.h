#pragma once
#include "SceneBase.h"

class AimCursor;
class Player;

enum STAGE_TYPE {
	STAGE_PLANE = 0,
	STAGE_1,
	STAGE_2,
};

class PlayScene : public SceneBase
{
	int time_;					//�o�ߎ���
	static STAGE_TYPE stageType_;		//���̃X�e�[�W�̃^�C�v�i���ꂩ��o���X�e�[�W�̃^�C�v�j

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

	static void SetStageType(STAGE_TYPE stageType) { stageType_ = stageType; }
	static STAGE_TYPE GetStageType() { return stageType_; }

	void SetAimCursor(int index, AimCursor* point) { pAimCursor_[index] = point; }
	AimCursor* GetAimCursor(int index) { return pAimCursor_[index]; }

};