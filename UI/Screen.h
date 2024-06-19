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

	//UIの追加
	void AddUI(std::string name, UI_TYPE type, XMFLOAT2 pos, std::function<void()> onClick);
	void AddUI(std::string name, UI_TYPE type, XMFLOAT2 pos, XMFLOAT2 size, std::function<void()> onClick);
	//UIの追加（描画画像のサイズ指定込みの関数）
	void AddUI(std::string name, UI_TYPE type, XMFLOAT2 pos, XMFLOAT2 size, std::function<void()> onClick, XMFLOAT2 tsize);

	//Ui削除
	bool DeleteUI(UIBase* ui);

	UI_STATE GetUIState() { return state_; };

};