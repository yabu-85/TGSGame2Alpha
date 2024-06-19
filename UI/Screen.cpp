#include "Screen.h"
#include "UIBase.h"
#include "ButtonUI.h"
#include "../Engine/Input.h"

Screen::Screen() : state_(UI_STATE::DRAW)
{
}

Screen::~Screen()
{
	for (auto u : uiList_)
	{
		delete u;
	}
	uiList_.clear();
}

void Screen::Update()
{
	for (auto u : uiList_)
	{
		if (u->IsWithinBound()) {
			if (Input::IsMouseButtonUp(0)) {
				u->OnClick();
			}
		}
	}

}

void Screen::Draw()
{
	for (auto u : uiList_)
	{
		u->Draw();
	}
}

void Screen::AddUi(std::string name, UI_TYPE type, XMFLOAT2 pos, std::function<void()> onClick)
{
	XMFLOAT2 size = { 1.0f, 1.0f };
	AddUi(name, type, pos, size, onClick);
}

void Screen::AddUi(std::string name, UI_TYPE type, XMFLOAT2 pos, XMFLOAT2 size, std::function<void()> onClick)
{
	//インスタンス生成
	UIBase* ui = nullptr;
	switch (type)
	{
	case UI_BUTTON: ui = new ButtonUI; break;
	case UI_MAX: break;
	default: break;
	}
	if (!ui) return;

	//生成できたからUIの初期化と追加
	ui->Initialize(name, pos, size, onClick);
	uiList_.push_back(ui);
}
