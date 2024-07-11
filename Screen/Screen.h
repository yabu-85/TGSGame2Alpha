#pragma once
#include <vector>
#include <string>
#include <DirectXMath.h>
#include<functional>
#include "../Engine/Transform.h"

using namespace DirectX;
class UIBase;

enum UI_TYPE {
	UI_BUTTON = 0,
	UI_SLIDER,
	UI_MAX,
};

enum SCREEN_STATE {
	DRAW = 0,
	ENDDRAW,
};

class Screen
{
protected:
	std::vector<UIBase*> uiList_;
	SCREEN_STATE state_;
	Transform transform_;

public:
	Screen();
	virtual ~Screen();
	virtual void Update();
	virtual void Draw();

	//UI�̒ǉ�
	UIBase* AddUI(std::string name, UI_TYPE type, XMFLOAT2 pos, std::function<void()> onClick);
	UIBase* AddUI(std::string name, UI_TYPE type, XMFLOAT2 pos, XMFLOAT2 size, std::function<void()> onClick);
	//UI�̒ǉ��i�`��摜�̃T�C�Y�w�荞�݂̊֐��j
	UIBase* AddUI(std::string name, UI_TYPE type, XMFLOAT2 pos, XMFLOAT2 size, std::function<void()> onClick, XMFLOAT2 tsize);

	//Ui�폜
	bool DeleteUI(UIBase* ui);

	SCREEN_STATE GetUIState() { return state_; };

};