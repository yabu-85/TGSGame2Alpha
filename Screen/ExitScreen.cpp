#include "ExitScreen.h"
#include "../Engine/Image.h"
#include "../Engine/SceneManager.h"
#include "../UI/UIBase.h"
#include "../UI/ButtonUI.h"

ExitScreen::ExitScreen() : Screen(), hPict_{ -1, -1 }
{
	//ImageÇÃèâä˙ê›íË
	const char* fileName[] = { "Image/ExitGame.png", "Image/BlackFade.png" };
	for (int i = 0; i < 2; i++) {
		hPict_[i] = Image::Load(fileName[i]);
		assert(hPict_[i] >= 0);
	}

	Transform t;
	t.position_ = XMFLOAT3(0.0f, 0.5f, 0.0f);
	t.scale_ = XMFLOAT3(0.5f, 0.5f, 0.0f);
	Image::SetTransform(hPict_[0], t);
	Image::SetFullScreenTransform(hPict_[1]);
	Image::SetAlpha(hPict_[1], 100);

	//É{É^Éììoò^
	UIBase* ui = nullptr;
	
	//Ok
	ui = ui->UIInstantiate<ButtonUI>("Ok", XMFLOAT2(0.0f, 0.0f), XMFLOAT2(0.4f, 0.35f), XMFLOAT2(0.4f, 0.4f), [this]()
		{
			PostQuitMessage(0);
		});
	AddUI(ui);
	ui->SetSelect(true);

	//ñﬂÇË
	AddUI(ui->UIInstantiate<ButtonUI>("Back", XMFLOAT2(0.0f, -0.5f), XMFLOAT2(0.4f, 0.35f), XMFLOAT2(0.4f, 0.4f), [this]()
		{
			state_ = ENDDRAW;
		}));

}

ExitScreen::~ExitScreen()
{
}

void ExitScreen::Draw()
{
	Image::Draw(hPict_[1]);
	Image::Draw(hPict_[0]);

	Screen::Draw();

}