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
	
	Transform buttonTransform_;
	Transform uiTransform_;
	std::function<void()> onClick_;		//UI����������ĂԊ֐�

	enum BUTTON_STATE {
		PRESSED = 0,
		UPPRESSED,
	};

public:
	UIBase();
	virtual ~UIBase();
	void Draw();
	void OnClick();

	virtual void Initialize() {};
	void Initialize(std::string name, std::function<void()> onClick);
	void Initialize(std::string name, XMFLOAT2 pos, XMFLOAT2 size, std::function<void()> onClick);

	virtual bool IsWithinBound() = 0;

};

