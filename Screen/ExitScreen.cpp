#include "ExitScreen.h"
#include "../Engine/Image.h"
#include "../Engine/SceneManager.h"
#include "../UI/UIBase.h"
#include "../UI/ButtonUI.h"

ExitScreen::ExitScreen() : Screen(), hPict_{ -1, -1 }
{
	//ImageÇÃèâä˙ê›íË
	const char* fileName[] = { "Image/ExitGame.png", "Image/WhiteFade.png" };
	for (int i = 0; i < 2; i++) {
		hPict_[i] = Image::Load(fileName[i]);
		assert(hPict_[i] >= 0);
	}

	Transform t;
	t.position_ = XMFLOAT3(0.0f, 0.5f, 0.0f);
	t.scale_ = XMFLOAT3(0.5f, 0.5f, 0.0f);
	Image::SetTransform(hPict_[0], t);
	Image::SetFullScreenTransform(hPict_[1]);
	Image::SetAlpha(hPict_[0], 150);
	Image::SetAlpha(hPict_[1], 150);

	//É{É^Éììoò^
	UIBase* ui = nullptr;
	
	//Ok
	ui = ui->UIInstantiate<ButtonUI>("Ok", XMFLOAT2(0.0f, 0.0f), XMFLOAT2(0.2f, 0.2f), XMFLOAT2(0.2f, 0.2f), [this]()
		{
			PostQuitMessage(0);
		});
	AddUI(ui);
	ui->SetSelect(true);

	//ñﬂÇË
	AddUI(ui->UIInstantiate<ButtonUI>("Back", XMFLOAT2(0.0f, -0.5f), XMFLOAT2(0.2f, 0.2f), XMFLOAT2(0.2f, 0.2f), [this]()
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