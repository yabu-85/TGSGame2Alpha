#include "SelectScreen.h"
#include "../Engine/Image.h"
#include "../Engine/SceneManager.h"

SelectScreen::SelectScreen() : Screen(), hPict_{ -1, -1 }
{
	const char* fileName[] = { "Image/Pause.png", "Image/WhiteFade.png" };
	for (int i = 0; i < 2; i++) {
		hPict_[i] = Image::Load(fileName[i]);
		assert(hPict_[i] >= 0);
	}

	transform_.position_ = { 0.0f, 0.0f, 1.0f };
	Image::SetTransform(hPict_[0], transform_);
	Image::SetFullScreenTransform(hPict_[1]);

	AddUI("A", UI_BUTTON, XMFLOAT2(-0.5f, 0.0f), XMFLOAT2(0.5, 0.2f), [this]() { if (!uiList_.empty()); }, XMFLOAT2(0.1f, 0.05f));
	AddUI("B", UI_BUTTON, XMFLOAT2(0.5f, 0.0f), XMFLOAT2(0.5, 0.2f), [this]() { if (!uiList_.empty()); }, XMFLOAT2(0.1f, 0.05f));
	
	AddUI("B", UI_SLIDER, XMFLOAT2(-0.25f, 0.5f), XMFLOAT2(1.0f, 1.0f), [this]() { if(!uiList_.empty()) ; }, XMFLOAT2(0.6f, 0.6f));

}

SelectScreen::~SelectScreen()
{
}

void SelectScreen::Update()
{
	Screen::Update();

}

void SelectScreen::Draw()
{
	Screen::Draw();

//	Image::Draw(hPict_[1]);
//	Image::Draw(hPict_[0]);

}