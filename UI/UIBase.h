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

	Transform frameTransform_;
	Transform imageTransform_;
	std::function<void()> onClick_;		//UIを押したら呼ぶ関数

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

