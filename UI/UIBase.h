#pragma once
#include <DirectXMath.h>
#include <string>
#include <functional>
#include "../Engine/Transform.h"

using namespace DirectX;

class UIBase
{
protected:
	int hButtonPict_[2];				//UI�{�̂̉摜�n���h��
	int hImagePict_;					//�\������摜�̃n���h��
	int alpha_;							//�����x
	bool isBound_;						//�͈͂̓����ɂ��邩�ǂ���

	Transform frameTransform_;
	Transform imageTransform_;
	std::function<void()> onClick_;		//UI����������ĂԊ֐�

	enum BUTTON_STATE {
		PRESSED = 0,
		UPPRESSED,
	};


public:
	UIBase(XMFLOAT2 pos, XMFLOAT2 size, std::function<void()> onClick, XMFLOAT2 tsize);
	virtual ~UIBase();
	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual void Initialize(std::string name) = 0;
	virtual bool IsWithinBound() = 0;
	virtual void OnClick();

};

