#include "PauseScreen.h"
#include "ScreenManager.h"
#include "../Engine/Image.h"
#include "../Engine/SceneManager.h"

//É{É^ÉìâüÇµÇΩÇ∆Ç´ÇÃèàóù
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

	hPict_ = Image::Load("Image/cross.png");
	assert(hPict_ >= 0);

	pauseTrans_.position_ = { 0.0f, 0.0f, 1.0f };

	AddUI("ReturnGame", UI_BUTTON, XMFLOAT2(0.0f, 0.8f), XMFLOAT2(0.5, 0.2f), [this]() { if(!uiList_.empty()) Kill(uiList_.at(0)); }, XMFLOAT2(0.33f, 0.33f));

}

PauseScreen::~PauseScreen()
{
}

void PauseScreen::Draw()
{
	Screen::Draw();

	return;

	Image::SetFullScreenTransform(hPict_[1]);
	Image::SetAlpha(hPict_[1], 150);
	Image::Draw(hPict_[1]);

	Image::SetTransform(hPict_[0], pauseTrans_);
	Image::Draw(hPict_[0]);

	Transform t;
	t.scale_ = XMFLOAT3(0.3f, 0.3f, 0.3f);
	Direct3D::SetBlendMode(Direct3D::BLEND_DEFAULT);
	Direct3D::SetDepthBafferWriteEnable(false);
	Image::SetTransform(hPict_, t);
	Image::Draw(hPict_);
	Direct3D::SetBlendMode(Direct3D::BLEND_ADD);
	Direct3D::SetDepthBafferWriteEnable(true);

}