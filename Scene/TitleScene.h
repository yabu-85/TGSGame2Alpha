#pragma once
#include "SceneBase.h"

class TitleScene : public SceneBase
{
	int time_;				//�o�ߎ���
	int stageModel_;		//�\������Stage�̔ԍ�
	float cameraRotate_;	//�J������]�v�Z�p�ϐ�
	XMFLOAT3 lightPos_[2];	//���C�g�̃|�W�V����

public:
	TitleScene(GameObject* parent);
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Release() override;
};