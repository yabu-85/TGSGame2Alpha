#include "PauseScreen.h"
#include "ScreenManager.h"
#include "../Engine/Image.h"
#include "../Engine/SceneManager.h"

//É{É^ÉìâüÇµÇΩÇ∆Ç´ÇÃèàóù
static void Kill(UIBase* ui) {
	ScreenManager::DeleteUI(ui);

}

PauseScreen::PauseScreen() : Screen(), hPict_{ -1, -1 }
{
	const char* fileName[] = { "Image/Pause.png", "Image/WhiteFade.png" };
	for (int i = 0; i < 2; i++) {
		hPict_[i] = Image::Load(fileName[i]);
		assert(hPict_[i] >= 0);
	}

	hCurPict_ = Image::Load("Image/cross.png");
	assert(hCurPict_ >= 0);
	
	Transform t;
	t.scale_ = XMFLOAT3(0.3f, 0.3f, 0.3f);
	Image::SetTransform(hCurPict_, t);

	transform_.position_ = { 0.0f, 0.0f, 1.0f };
	Image::SetTransform(hPict_[0], transform_);
	Image::SetFullScreenTransform(hPict_[1]);

	AddUI("ReturnGame", UI_BUTTON, XMFLOAT2(0.0f, 0.8f), XMFLOAT2(0.5, 0.2f), [this]() { if(!uiList_.empty()) Kill(uiList_.at(0)); }, XMFLOAT2(0.33f, 0.33f));

}

PauseScreen::~PauseScreen()
{
}

void PauseScreen::Draw()
{
	Screen::Draw();
	Image::Draw(hCurPict_);
	
	return;

	Image::Draw(hPict_[1]);
	Image::Draw(hPict_[0]);

}