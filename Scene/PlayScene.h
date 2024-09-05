#pragma once
#include "SceneBase.h"

class AimCursor;
class Player;
class BulletInfoDisplay;

enum STAGE_TYPE {
	STAGE_PLANE = 0,
	STAGE_1,
	STAGE_2,
};

class PlayScene : public SceneBase
{
	bool isPause_;					//Pause中かどうか
	bool preStageDraw_;				//戦闘が始まる前のステージ描画中か
	int time_;						//経過時間
	int endTime_;					//終了待ち時間

	XMFLOAT3 preDrawCameraPos_;		//始まる前描画のカメラPosition
	XMFLOAT3 preDrawCameraTar_;		//始まる前描画のカメラTarget

	Player* pPlayer_[2];
	AimCursor* pAimCursor_[2];
	BulletInfoDisplay* pBulletInfoDisplay_[2];

	static STAGE_TYPE stageType_;	//今のステージのタイプ（これから出すステージのタイプ）

	bool IsPauseButtonDown();

public:
	PlayScene(GameObject* parent);
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Release() override;
	void CommonUIDraw() override;
	void IndividualUIDraw(int index) override;

	//StageTypeのセット
	static void SetStageType(STAGE_TYPE stageType) { stageType_ = stageType; }
	static STAGE_TYPE GetStageType() { return stageType_; }

	void SetAimCursor(int index, AimCursor* point) { pAimCursor_[index] = point; }
	AimCursor* GetAimCursor(int index) { return pAimCursor_[index]; }

};