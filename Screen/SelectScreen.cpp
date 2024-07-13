#include "SelectScreen.h"
#include "../Engine/Image.h"
#include "../Engine/SceneManager.h"
#include "../UI/UIBase.h"
#include "../Scene/SceneBase.h"
#include "../Screen/PauseScreen.h"
#include "../Other/GameManager.h"

SelectScreen::SelectScreen() : Screen(), hPict_{ -1, -1 }
{
	const char* fileName[] = { "Image/Pause.png", "Image/WhiteFade.png" };
	for (int i = 0; i < 2; i++) {
		hPict_[i] = Image::Load(fileName[i]);
		assert(hPict_[i] >= 0);
	}

	Transform t;
	Image::SetTransform(hPict_[0], t);
	Image::SetFullScreenTransform(hPict_[1]);

	UIBase* ui = AddUI("A", UI_BUTTON, XMFLOAT2(-0.5f, 0.0f), XMFLOAT2(0.5, 0.2f), [this]() { if (!uiList_.empty()); }, XMFLOAT2(0.1f, 0.05f));
	if (ui) ui->SetSelect(true);

	AddUI("B", UI_BUTTON, XMFLOAT2(0.5f, 0.0f), XMFLOAT2(0.5, 0.2f), [this]() { 
		GameManager::GetScene()->AddScreen(new PauseScreen());
	}, XMFLOAT2(0.1f, 0.05f));
	
	AddUI("B", UI_SLIDER, XMFLOAT2(-0.3f, 0.5f), XMFLOAT2(0.3f, 1.0f), [this]() { if(!uiList_.empty()) ; }, XMFLOAT2(0.6f, 0.6f));
	AddUI("B", UI_SLIDER, XMFLOAT2(0.3f, 0.5f), XMFLOAT2(0.3f, 1.0f), [this]() { if(!uiList_.empty()) ; }, XMFLOAT2(0.6f, 0.6f));

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