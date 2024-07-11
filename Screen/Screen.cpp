#include "Screen.h"
#include "../UI/UIBase.h"
#include "../UI/ButtonUI.h"
#include "../UI/SliderUI.h"
#include "../Engine/Input.h"

Screen::Screen() : state_(SCREEN_STATE::DRAW)
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
		u->Update();
	}
}

void Screen::Draw()
{
	for (auto u : uiList_)
	{
		u->Draw();
	}
}

void Screen::AddUI(std::string name, UI_TYPE type, XMFLOAT2 pos, std::function<void()> onClick)
{
	XMFLOAT2 size = { 1.0f, 1.0f };
	AddUI(name, type, pos, size, onClick);
}

void Screen::AddUI(std::string name, UI_TYPE type, XMFLOAT2 pos, XMFLOAT2 size, std::function<void()> onClick)
{
	AddUI(name, type, pos, size, onClick, XMFLOAT2(1.0f, 1.0f));
}

void Screen::AddUI(std::string name, UI_TYPE type, XMFLOAT2 pos, XMFLOAT2 size, std::function<void()> onClick, XMFLOAT2 tsize)
{
	//インスタンス生成
	UIBase* ui = nullptr;
	switch (type)
	{
	case UI_BUTTON: ui = new ButtonUI(pos, size, onClick, tsize); break;
	case UI_SLIDER: ui = new SliderUI(pos, size, onClick, tsize); break;
	}
	if (!ui) return;

	//生成できたからUIの初期化と追加
	ui->Initialize(name);
	uiList_.push_back(ui);
}

bool Screen::DeleteUI(UIBase* ui)
{
	for (auto iter = uiList_.begin(); iter != uiList_.end();) {
		if ((*iter) == ui)
		{
			delete* iter;
			iter = uiList_.erase(iter);
			return true;
		}
	}
	return false;
}
