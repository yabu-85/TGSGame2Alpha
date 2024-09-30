#include "Screen.h"
#include "../UI/UIBase.h"
#include "../UI/ButtonUI.h"
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
	case UI_BUTTON: ui = new ButtonUI(); break;
	}
	if (!ui) return;

	//生成できたからUIの初期化と追加
	ui->Initialize(name, pos, size, onClick, tsize);
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
