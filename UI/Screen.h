#pragma once
#include <vector>
#include <string>
#include <DirectXMath.h>
#include<functional>

using namespace DirectX;
class UIBase;

enum UI_TYPE {
	UI_BUTTON = 0,
	UI_MAX,
};

class Screen
{
public:
	enum UI_STATE {
		DRAW = 0,
		ENDDRAW,
	};

protected:
	std::vector<UIBase*> uiList_;
	UI_STATE state_;

public:
	Screen();
	virtual ~Screen();
	virtual void Update();
	virtual void Draw();

	//ボタンの追加
	void AddUi(std::string name, UI_TYPE type, XMFLOAT2 pos, std::function<void()> onClick);
	//ボタンの追加
	void AddUi(std::string name, UI_TYPE type, XMFLOAT2 pos, XMFLOAT2 size, std::function<void()> onClick);

	UI_STATE GetUIState() { return state_; };

};