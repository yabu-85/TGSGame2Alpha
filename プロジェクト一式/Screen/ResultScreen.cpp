#include "ResultScreen.h"
#include "../Engine/Image.h"
#include "../Engine/SceneManager.h"
#include "../Engine/Camera.h"
#include "../Other/GameManager.h"
#include "../UI/UIBase.h"
#include "../UI/ButtonUI.h"

namespace {
	const char* fileName[] = { "Image/Player1.png", "Image/Player2.png", "Image/Win.png", "Image/Win.png" };
	const XMFLOAT3 WIN_IMAGE_POS = XMFLOAT3(0.0f, 0.25f, 0.0f);
	const XMFLOAT3 PLAYER_IMAGE_POS = XMFLOAT3(0.0f, 0.5f, 0.0f);

}

ResultScreen::ResultScreen() : Screen(), winPlayerID_(-1)
{
	//画像初期化
	for (int i = 0; i < MAX; i++) { hPict_[i] = -1; }
	hPict_[WIN] = Image::Load(fileName[3]);
	assert(hPict_[WIN] >= 0);

	Transform t;
	t.position_ = WIN_IMAGE_POS;
	Image::SetTransform(hPict_[WIN], t);

	//ボタン登録
	UIBase* ui = nullptr;
	
	//戻り
	ui = ui->UIInstantiate<ButtonUI>("Back", XMFLOAT2(0.0f, -0.5f), XMFLOAT2(0.4f, 0.35f), XMFLOAT2(0.4f, 0.4f), [this]()
		{
			state_ = ENDDRAW;
		});
	AddUI(ui);
	ui->SetSelect(true);

}

ResultScreen::~ResultScreen()
{
}

void ResultScreen::Draw()
{
	//画面分割無しに
	Camera::SetOneProjectionMatrix();
	Direct3D::SetViewOne();
	Direct3D::SetViewPort(0);

	Image::Draw(hPict_[WIN]);
	Image::Draw(hPict_[PLAYER]);

	Screen::Draw();

	//画面分割2に戻す
	if (!GameManager::IsOnePlayer()) {
		Camera::SetTwoProjectionMatrix();
		Direct3D::SetViewTwo();
		Direct3D::SetViewPort(0);
	}
}

void ResultScreen::SetWinPlayer(int i)
{
	//エラー処理
	if (!(i == 0 || i == 1)) {
		hPict_[PLAYER] = Image::Load(fileName[2]);
		assert(hPict_[PLAYER] >= 0);
	}
	else {
		winPlayerID_ = i;
		hPict_[PLAYER] = Image::Load(fileName[winPlayerID_]);
		assert(hPict_[PLAYER] >= 0);
	}

	Transform t;
	t.position_ = PLAYER_IMAGE_POS;
	Image::SetTransform(hPict_[PLAYER], t);

}
