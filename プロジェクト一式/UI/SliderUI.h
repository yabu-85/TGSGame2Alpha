#pragma once
#include "UIBase.h"

class SliderUI : public UIBase
{
	float gaugePercent_;	//�Q�[�W�p�[�Z���g
	float buttonPosX_;		//X���̃{�^���̍��W�i0~1
	bool isDragging_;		//�{�^���������Ă��邩
	XMFLOAT2 frameSize_;	//�t���[���̃T�C�Y

	//�X���C�_�[�������Ă���Ƃ��̏���
	void Dragging();

public:
	SliderUI(XMFLOAT2 pos, XMFLOAT2 size, std::function<void()> onClick, XMFLOAT2 tsize);
	~SliderUI() override;
	void Update() override;
	void Draw() override;
	void Initialize(std::string name) override;
	bool IsWithinBound() override;
	void SelectUpdate() override;

	//�O�`�P�̊ԂŎ擾
	float GetGaugeParcent() { return gaugePercent_; }

	void SetGaugeParcent(float f);

};

