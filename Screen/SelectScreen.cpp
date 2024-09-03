#include "SelectScreen.h"
#include "../Engine/SceneManager.h"
#include "../Scene/SceneBase.h"
#include "../Screen/SettingScreen.h"
#include "../Screen/PauseScreen.h"
#include "../Other/GameManager.h"
#include "../UI/ButtonUI.h"
#include "../Scene/PlayScene.h"
#include "../Engine/Image.h"

SelectScreen::SelectScreen() : Screen(), hPict_( -1)
{
	//Imageの初期設定
	const char* fileName = { "Image/StageSelect.png" };
	hPict_ = Image::Load(fileName);
	assert(hPict_ >= 0);

	Transform t;
	t.position_ = XMFLOAT3(0.0f, 0.5f, 0.0f);
	t.scale_ = XMFLOAT3(0.5f, 0.5f, 0.0f);
	Image::SetTransform(hPict_, t);

	UIBase* ui = nullptr;

	//Stage1
	ui = ui->UIInstantiate<ButtonUI>("Stage1", XMFLOAT2(-0.5f, 0.0f), XMFLOAT2(0.4f, 0.4f), XMFLOAT2(0.3f, 0.3f), [this]()
		{
			//ステージタイプセット
			PlayScene::SetStageType(STAGE_1);

			SceneManager* pSceneManager = (SceneManager*)GameManager::GetRootObject()->FindObject("SceneManager");
			pSceneManager->ChangeScene(SCENE_ID_PLAY);
		});
	AddUI(ui);
	ui->SetSelect(true);

	//Stage2
	AddUI(ui->UIInstantiate<ButtonUI>("Stage2", XMFLOAT2(0.5f, 0.0f), XMFLOAT2(0.4f, 0.4f), XMFLOAT2(0.3f, 0.3f), [this]()
		{
			//ステージタイプセット
			PlayScene::SetStageType(STAGE_2);
			
			SceneManager* pSceneManager = (SceneManager*)GameManager::GetRootObject()->FindObject("SceneManager");
			pSceneManager->ChangeScene(SCENE_ID_PLAY);
		}));

	//戻り
	AddUI(ui->UIInstantiate<ButtonUI>("Back", XMFLOAT2(0.0f, -0.5f), XMFLOAT2(0.3f, 0.3f), XMFLOAT2(0.4f, 0.4f), [this]()
		{
			state_ = ENDDRAW;
		}));

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
	Image::Draw(hPict_);

	Screen::Draw();

}