#pragma once
#include "SceneBase.h"

class TitleScene : public SceneBase
{
	int time_;				//経過時間
	int stageModel_;		//表示するStageの番号
	float cameraRotate_;	//カメラ回転計算用変数
	XMFLOAT3 lightPos_[2];	//ライトのポジション

public:
	TitleScene(GameObject* parent);
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Release() override;
};