#include "PauseScreen.h"
#include "../Engine/Image.h"
#include "../Engine/SceneManager.h"
#include "../UI/UIBase.h"

PauseScreen::PauseScreen() : Screen(), hPict_{ -1, -1 }
{
	const char* fileName[] = { "Image/Pause.png", "Image/WhiteFade.png" };
	for (int i = 0; i < 2; i++) {
		hPict_[i] = Image::Load(fileName[i]);
		assert(hPict_[i] >= 0);
	}

	Transform t;
	Image::SetTransform(hPict_[0], t);
	Image::SetFullScreenTransform(hPict_[1]);
	Image::SetAlpha(hPict_[0], 150);
	Image::SetAlpha(hPict_[1], 150);

	UIBase* ui = AddUI("ReturnGame", UI_BUTTON, XMFLOAT2(0.0f, 0.8f), XMFLOAT2(0.5, 0.2f), [this]() { if(!uiList_.empty()) state_ = ENDDRAW; }, XMFLOAT2(0.33f, 0.33f));
	if (ui) ui->SetSelect(true);

}

PauseScreen::~PauseScreen()
{
}

void PauseScreen::Draw()
{
	Screen::Draw();
	
	Image::Draw(hPict_[1]);
	Image::Draw(hPict_[0]);

}