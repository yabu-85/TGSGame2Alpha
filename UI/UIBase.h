#pragma once
#include <DirectXMath.h>
#include <string>
#include <functional>
#include "../Engine/Transform.h"

using namespace DirectX;

class UIBase
{
protected:
	int hButtonPict_[2];				//UI本体の画像ハンドル
	int hImagePict_;					//表示する画像のハンドル
	int alpha_;							//透明度
	bool isBound_;						//範囲の内側にいるかどうか
	
	Transform buttonTransform_;
	Transform uiTransform_;
	std::function<void()> onClick_;		//UIを押したら呼ぶ関数

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

