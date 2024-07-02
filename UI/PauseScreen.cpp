#include "PauseScreen.h"
#include "ScreenManager.h"
#include "../Engine/Image.h"
#include "../Engine/SceneManager.h"

//ƒ{ƒ^ƒ“‰Ÿ‚µ‚½‚Æ‚«‚Ìˆ—
void Kill(UIBase* ui) {
	ScreenManager::DeleteUI(ui);

}

PauseScreen::PauseScreen() : Screen(), hPict_{ -1, -1 }
{
	const char* fileName[] = { "Image/Pause.png", "Image/WhiteFade.png" };
	for (int i = 0; i < 2; i++) {
		hPict_[i] = Image::Load(fileName[i]);
		assert(hPict_[i] >= 0);
	}

	pauseTrans_.position_ = { 0.0f, 0.0f, 1.0f };
	Image::SetTransform(hPict_[0], pauseTrans_);
	Image::SetFullScreenTransform(hPict_[1]);

	AddUI("ReturnGame", UI_BUTTON, XMFLOAT2(0.0f, 0.8f), XMFLOAT2(0.5, 0.2f), [this]() { if(!uiList_.empty()) Kill(uiList_.at(0)); }, XMFLOAT2(0.33f, 0.33f));

}

PauseScreen::~PauseScreen()
{
}

void PauseScreen::Draw()
{
	Image::Draw(hPict_[1]);
	Image::Draw(hPict_[0]);

}