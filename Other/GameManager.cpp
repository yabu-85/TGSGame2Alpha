#include "GameManager.h"
#include "../Engine/Direct3D.h"
#include "../Engine/SceneManager.h"
#include "../Engine/RootObject.h"
#include "../Engine/Light.h"
#include "../Stage/CollisionMap.h"
#include "../Player/Player.h"
#include "../Other/InputManager.h"
#include "../UI/DamageUI.h"
#include "../Scene/SceneBase.h"
#include "../Json/JsonReader.h"

//ImGui
#include "../Engine/ImGui/imgui.h"
#include "../Engine/ImGui/imgui_impl_dx11.h"
#include "../Engine/ImGui/imgui_impl_win32.h"
#include "../Engine/Model.h"
#include "../Enemy/EnemyManager.h"
#include "../Enemy/EnemyBase.h"
#include "../State/StateManager.h"
#include "../Weapon/GunBase.h"
#include <vector>
#include <string>

namespace GameManager {
	//ImGui関連
	float playerSpeed = 0.0f;
	bool playerClimb = false;
	bool playerFaly = false;
	std::vector<EnemyBase*> enemies;
	//ImGui選択
	int selectedIndex = -1;
	enum class SelectedType { None, Player1, Player2, Enemy };
	SelectedType selectedType = SelectedType::None;

	//GameManager
	bool isImGuiDraw_ = true;
	bool isOnePlayer_ = true;
	bool isPCCtrl_ = false;
	bool cursorMode_ = false;
	int pcCtrlNumber_ = 0;
	int drawIndex_ = 0;
	
	//影関係の情報
	XMFLOAT3 SHADOW_CAMERA_TARGET = XMFLOAT3(50.0f, 0.0f, 50.0f);
	bool isShadowDraw_ = false;

	GameObject* pRootObject_ = nullptr;
	CollisionMap* pCollisionMap_ = nullptr;
	Player* pPlayer_[2] = { nullptr, nullptr };
	SceneBase* pScene_ = nullptr;

	void Initialize()
	{
		InputManager::Initialize();

		//GameSetting読み込み
		JsonReader::Load("Json/GameSetting.json");
		auto& commonSection = JsonReader::GetSection("Common");
		auto& player1Section = JsonReader::GetSection("Player1");
		auto& player2Section = JsonReader::GetSection("Player2");
		int pcCtrl1 = player1Section["pcCtrl"];
		int pcCtrl2 = player2Section["pcCtrl"];
		int shadowDraw = commonSection["shadowDraw"];

		//Ctrl
		if (pcCtrl1 || pcCtrl2) {
			isPCCtrl_ = true;
			if (pcCtrl1 == 1) pcCtrlNumber_ = 0;
			else if (pcCtrl2 == 1) pcCtrlNumber_ = 1;
		}

		//画面分割
		if (isOnePlayer_) {
			Direct3D::SetViewOne();
			Direct3D::SetViewPort(0);
			Camera::SetOneProjectionMatrix();
		}
		else {
			Direct3D::SetViewTwo();
			Camera::SetTwoProjectionMatrix();
		}

		//Shadow
		isShadowDraw_ = shadowDraw;

	}

	void Update()
	{
		DamageUI::Update();
		enemies = EnemyManager::GetAllEnemy();

	}

	void Draw()
	{
		if (isOnePlayer_) OnePlayerDraw();
		else TwoPlayerDraw();
	}

	void SceneChange()
	{
		pCollisionMap_ = nullptr;
		for(int i = 0;i < 2;i++) pPlayer_[i] = nullptr;
		pScene_ = nullptr;

		DamageUI::SceneChange();
	}

	//今描画中の番号
	int GetDrawIndex() { return drawIndex_; }

	//メニュ状態か
	bool IsCursorMode() { return cursorMode_; }
	void SetCursorMode(bool b) { cursorMode_ = b; }
	
	//人数関係
	void SetOnePlayer() { isOnePlayer_ = true; }
	void SetTwoPlayer() { isOnePlayer_ = false; }
	bool IsOnePlayer() { return isOnePlayer_; }

	//PCCtrl関係
	void SetPCCtrlON() { isPCCtrl_ = true; }
	void SetPCCtrlOFF() { isPCCtrl_ = false; }
	bool IsPCCtrl() { return isPCCtrl_; }
	void SetPCCtrlNumber(int number) { pcCtrlNumber_ = number; }
	int GetPCCtrlNumber() { return pcCtrlNumber_; }

	//影の設定
	void SetShadowDraw(bool b) { isShadowDraw_ = b; }
	bool IsShadowDraw() { return isShadowDraw_; }

	//アクセサ
	GameObject* GetRootObject() { return pRootObject_; }
	void SetRootObject(GameObject* root) { pRootObject_ = root; }
	CollisionMap* GameManager::GetCollisionMap() { return pCollisionMap_; }
	void GameManager::SetCollisionMap(CollisionMap* map) { pCollisionMap_ = map; }
	Player* GameManager::GetPlayer(int index) { return pPlayer_[index]; }
	void GameManager::SetPlayer(Player* player, int index) { pPlayer_[index] = player; }
	SceneBase* GetScene() { return pScene_; }
	void SetScene(SceneBase* stage) { pScene_ = stage; }

	//GameManagerの中で使う
	void CommonDraw()
	{
		if (Direct3D::GetCurrentShader() == Direct3D::SHADER_3D) {
			if(pScene_) pScene_->CommonUIDraw();
		}
	}

	void IndividualDraw(int index)
	{
		if (Direct3D::GetCurrentShader() == Direct3D::SHADER_3D) {
			if (pScene_) pScene_->IndividualUIDraw(index);
		}

		DamageUI::Draw(index);
	}



	void ImGuiDraw()
	{
		//ImGuiの更新処理
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin("Hello");

		//プレイヤーと敵のリストを表示するウィジェット
		ImGui::Text("Select an Entity:");
		ImGui::Separator();

		static int selectedPlayer = -1;
		static int selectedEnemy = -1;

		//プレイヤーのリスト
		const char* playerNames[] = { "Player 1", "Player 2" };
		if (ImGui::ListBox("Players", &selectedPlayer, playerNames, IM_ARRAYSIZE(playerNames)))
		{
			selectedType = (selectedPlayer == 0) ? SelectedType::Player1 : SelectedType::Player2;
			selectedIndex = -1;
			selectedEnemy = -1;
		}

		//敵のリスト
		std::vector<std::string> enemyNames;
		for (const auto& enemy : enemies)
		{
			enemyNames.push_back(enemy->GetObjectName());
		}

		std::vector<const char*> enemyNamesCStr;
		for (const auto& name : enemyNames)
		{
			enemyNamesCStr.push_back(name.c_str());
		}

		if (ImGui::ListBox("Enemies", &selectedEnemy, enemyNamesCStr.data(), (int)enemyNamesCStr.size()))
		{
			selectedType = SelectedType::Enemy;
			selectedIndex = -1;
			selectedPlayer = -1;
		}

		ImGui::Separator();

		//選択されたObjのステータスを表示
		switch (selectedType)
		{
		case SelectedType::Player1:
		{
			XMFLOAT3 position = XMFLOAT3();
			if (GameManager::GetPlayer(0)) position = GameManager::GetPlayer(0)->GetPosition();

			ImGui::Text("Player 1 Position: (%.2f, %.2f, %.2f)", position.x, position.y, position.z);
			ImGui::Text("Player 1 Speed: %.2f", playerSpeed);
			ImGui::Text("Player 1 Faly: %s", playerFaly ? "true" : "false");
			ImGui::Text("Player 1 Climb: %s", playerClimb ? "true" : "false");

			if (GameManager::GetPlayer(0))
			{
				std::string strName = GameManager::GetPlayer(0)->GetStateManager()->GetName();
				const char* stateName = strName.c_str();
				int animFrameUp = Model::GetAnimFrame(pPlayer_[0]->GetUpModelHandle());
				int animFrameDown = Model::GetAnimFrame(pPlayer_[0]->GetDownModelHandle());
				ImGui::Text("Player 1 State: %s", stateName);
				ImGui::Text("Player 1 AnimFrameUp: %i", animFrameUp);
				ImGui::Text("Player 1 AnimFrameDo: %i", animFrameDown);
				ImGui::Text("Player 1 Accuracy: %f", pPlayer_[0]->GetGun()->GetAccuracy());
			}

		}
		break;

		case SelectedType::Player2:
		{
			XMFLOAT3 position = XMFLOAT3();
			if (GameManager::GetPlayer(1)) position = GameManager::GetPlayer(1)->GetPosition();

			ImGui::Text("Player 2 Position: (%.2f, %.2f, %.2f)", position.x, position.y, position.z);
			ImGui::Text("Player 2 Speed: %.2f", playerSpeed);
			ImGui::Text("Player 2 Faly: %s", playerFaly ? "true" : "false");
			ImGui::Text("Player 2 Climb: %s", playerClimb ? "true" : "false");

			if (GameManager::GetPlayer(1))
			{
				std::string strName = GameManager::GetPlayer(1)->GetStateManager()->GetName();
				const char* stateName = strName.c_str();
				int animFrameUp = Model::GetAnimFrame(pPlayer_[1]->GetUpModelHandle());
				int animFrameDown = Model::GetAnimFrame(pPlayer_[1]->GetDownModelHandle());
				ImGui::Text("Player 1 State: %s", stateName);
				ImGui::Text("Player 1 AnimFrameUp: %i", animFrameUp);
				ImGui::Text("Player 1 AnimFrameDo: %i", animFrameDown);
				ImGui::Text("Player 1 Accuracy: %f", pPlayer_[1]->GetGun()->GetAccuracy());
			}

		}
		break;

		case SelectedType::Enemy:
		{
			for (size_t i = 0; i < enemies.size(); ++i)
			{
				const auto& enemy = enemies[i];
				ImGui::Text("Enemy %zu Position: (%.2f, %.2f, %.2f)", i + 1, enemy->GetPosition().x, enemy->GetPosition().y, enemy->GetPosition().z);
			}
		}
		break;

		case SelectedType::None:
			ImGui::Text("No selection.");
			break;
		}

		ImGui::End(); //ImGuiの処理を終了
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}

	void ShadowDraw()
	{	
		XMFLOAT3 pos = Camera::GetPosition(0);
		XMFLOAT3 tar = Camera::GetTarget(0);
		XMFLOAT4 lightPos = Light::GetPosition(0);
		Camera::SetPosition(XMFLOAT3(lightPos.x, lightPos.y, lightPos.z), 0);
		Camera::SetTarget(SHADOW_CAMERA_TARGET, 0);
		Camera::Update(0);
		Direct3D::lightViewMatrix = Camera::GetViewMatrix();

		Direct3D::BeginShadowDraw(drawIndex_);
		pRootObject_->DrawSub();
		Direct3D::EndDraw();
		Camera::SetPosition(pos, 0);
		Camera::SetTarget(tar, 0);
		Camera::Update(0);
	}

	void OnePlayerDraw()
	{
		drawIndex_ = 0;
		Direct3D::SetViewPort(0);
		Direct3D::SetViewOne();
		Camera::SetOneProjectionMatrix(Camera::GetFovAngleParcent(0));
		
		if (isShadowDraw_) ShadowDraw();
		
		Direct3D::BeginDraw();
		Camera::Update(0);
		pRootObject_->DrawSub();
		EFFEKSEERLIB::gEfk->Draw();
		GameManager::IndividualDraw(0);
		GameManager::CommonDraw();

		//ImGui
		if (isImGuiDraw_) ImGuiDraw();

		Direct3D::EndDraw();
	}

	void TwoPlayerDraw()
	{
		Direct3D::SetViewTwo();

		//影描画
		if (isShadowDraw_) {
			for (int i = 0; i < 2; i++) {
				drawIndex_ = i;
				Camera::SetTwoProjectionMatrix(Camera::GetFovAngleParcent(i));
				ShadowDraw();
			}
		}

		Direct3D::BeginDraw();
		for (int i = 0; i < 2; i++) {
			drawIndex_ = i;
			Direct3D::SetViewPort(i);
			Camera::Update(i);
			Camera::SetTwoProjectionMatrix(Camera::GetFovAngleParcent(i));

			pRootObject_->DrawSub();
			EFFEKSEERLIB::gEfk->Draw();
			GameManager::IndividualDraw(i);
		}

		Direct3D::SetViewOne();
		Camera::SetOneProjectionMatrix();
		GameManager::CommonDraw();

		//ImGui
		if (isImGuiDraw_) ImGuiDraw();
	
		Direct3D::EndDraw();
	}

}
