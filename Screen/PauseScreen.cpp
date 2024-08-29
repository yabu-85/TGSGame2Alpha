#include "PauseScreen.h"
#include "../Engine/Image.h"
#include "../Engine/SceneManager.h"
#include "../UI/UIBase.h"
#include "../Other/GameManager.h"
#include "../Engine/Camera.h"

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

}

PauseScreen::~PauseScreen()
{
}

void PauseScreen::Draw()
{
	//‰æ–Ê•ªŠ„–³‚µ‚É
	GameManager::SetOnePlayer();
	Camera::SetOneProjectionMatrix();
	Direct3D::SetViewOne();
	Direct3D::SetViewPort(0);

	Image::Draw(hPict_[1]);
	Image::Draw(hPict_[0]); 
	
	Screen::Draw();

	//‰æ–Ê•ªŠ„2‚É–ß‚·
	GameManager::SetTwoPlayer();
	Camera::SetTwoProjectionMatrix();
	Direct3D::SetViewTwo();
	Direct3D::SetViewPort(0);

}