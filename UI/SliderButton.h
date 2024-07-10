#pragma once
#include "UIBase.h"

class SliderButton : public UIBase
{
	int frameAlpha_;		//�����x

	int num_;				//�l
	int maxNum_;			//�l�̍ő�l

	float width_;			//�{�^���̕�
	float height_;			//�{�^���̍���
	float buttonPosX_;		//X���̃{�^���̍��W�i0~1
	bool isButtonInactive_; //�{�^�����������m���Ȃ�
	bool isFirstPoint;		//����Ə���
	bool isDragging_;		//�{�^���������Ă��邩
	std::string name_;		//�{�^���ɕ\������Png��
	XMFLOAT3 widePos_;		//-1�`1����Ȃ�,-1280�`1280�Ƃ��̍��W
	XMFLOAT3 frameSize_;	//Frame�̃T�C�Y
	Direct3D::BLEND_MODE mode_;

public:
	SliderButton(GameObject* parent);
	~SliderButton();
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Release() override;

	//����:���Wx,y,�T�C�Yx.y, �摜�̖��O
	void SetValue(float x, float y, float w, float h, std::string n);

	void SetAlpha(int i) { alpha_ = i; }; //0�i�����j�`255�i�\���j
	void SetFrameAlpha(int i) { frameAlpha_ = i; }; //0�i�����j�`255�i�\���j
	void SetActive(bool b) { isButtonInactive_ = b; };
	bool IsWithinBound();

	void SetNum(int _num, int _max) { num_ = _num; maxNum_ = _max; };
	int GetValue() { return num_; };

};

