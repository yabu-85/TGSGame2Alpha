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
#include "../Engine/Camera.h"
#include "../Enemy/EnemyManager.h"
#include "../Enemy/EnemyBase.h"
#include "../Stage/StageEditor.h"
#include "../State/StateManager.h"
#include "../Weapon/GunBase.h"
#include <vector>
#include <string>
#include <cstring> 
#include <fstream>
#include "../Engine/CapsuleCollider.h"
#include "../Enemy/TestBoss.h"
 
void SaveBoneAttachData(const std::string& filename, BoneAttachColliderData* data, int size)
{
	nlohmann::json j;

	for (int i = 0; i < size; i++) {
		nlohmann::json boneData;
		boneData["radius"] = data[i].radius;
		boneData["height"] = data[i].height;
		boneData["boneName"] = data[i].boneName;
		boneData["position"] = { data[i].position.x, data[i].position.y, data[i].position.z };
		boneData["rotation"] = { data[i].rotation.x, data[i].rotation.y, data[i].rotation.z };

		j["boneAttachData"].push_back(boneData);
	}

	// JSONデータをファイルに書き出し
	std::ofstream ofs(filename);
	if (ofs.is_open())
	{
		ofs << j.dump(4);  // インデント4スペースで整形して保存
		ofs.close();
	}
}

namespace GameManager {
	//ImGui情報
	bool isImGuiDraw_ = false;
	float playerSpeed = 0.0f;
	bool playerClimb = false;
	bool playerFaly = false;
	std::vector<EnemyBase*> enemies;
	
	//ImGui選択
	int selectedIndex = -1;
	enum class SelectedType { None, Player1, Player2, Enemy };
	SelectedType selectedType = SelectedType::None;

	//GameManager
	bool isOnePlayer_ = true;				//画面分割してるか（true：してない）
	bool isPCCtrl_ = false;					//PCでの操作をしているかどうか
	bool cursorMode_ = false;				//カーソルモードかどうか（メニュー状態か）
	bool isEffectUpdateInMenu = false;		//メニュー状態でもエフェクトのUpdateさせるか
	int pcCtrlNumber_ = 0;					//PC操作をするプレイヤー番号
	int drawIndex_ = 0;						//描画している画面の番号（計算用）
	
	//影関係の情報
	XMFLOAT3 SHADOW_CAMERA_TARGET = XMFLOAT3(50.0f, 0.0f, 50.0f);
	bool isShadowDraw_ = false;

	//ポインタまとめ
	GameObject* pRootObject_ = nullptr;
	CollisionMap* pCollisionMap_ = nullptr;
	Player* pPlayer_[2] = { nullptr, nullptr };
	SceneBase* pScene_ = nullptr;

	void Initialize()
	{
		InputManager::Initialize();

		//デバッグ時はImGui表示に
#if 1 //_DEBUG
		isImGuiDraw_ = true;
#endif

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
	void SetDrawIndex(int i) { drawIndex_ = i; }

	//メニュ状態か
	bool IsCursorMode() { return cursorMode_; }
	void SetCursorMode(bool b) { cursorMode_ = b; }

	//メニュー状態でもエフェクトのアップデートさせるか
	bool IsEffectUpdateInMenu() { return isEffectUpdateInMenu; }
	void SetEffectUpdateInMenu(bool b) { isEffectUpdateInMenu = b; }
	
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

		// シャドウのオンオフ切り替えボタン
		if (ImGui::Button("Toggle Shadow"))
		{
			isShadowDraw_ = !isShadowDraw_;
		}

		// 画面分割のオンオフ切り替えボタン
		if (ImGui::Button("Toggle Screen Split"))
		{
			isOnePlayer_ = !isOnePlayer_;
			if (isOnePlayer_) {
				Direct3D::SetViewOne();
				Direct3D::SetViewPort(0);
				Camera::SetOneProjectionMatrix();
			}
			else {
				Direct3D::SetViewTwo();
				Camera::SetTwoProjectionMatrix();
			}
		}

		// drawIndex の切り替え
		if (ImGui::Button("Toggle Screen Draw Index"))
		{
			if (isOnePlayer_) {
				if (drawIndex_ == 0) drawIndex_ = 1;
				else drawIndex_ = 0;
			}
		}

		ImGui::Separator();

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
			selectedIndex = selectedEnemy;
		}


		ImGui::Separator();

		//選択されたObjのステータスを表示
		switch (selectedType)
		{
		case SelectedType::Player1:
		{
			XMFLOAT3 position = XMFLOAT3();
			if (GameManager::GetPlayer(0)) position = GameManager::GetPlayer(0)->GetPosition();

			ImGui::Text("Player 1 Position: (%.3f, %.3f, %.3f)", position.x, position.y, position.z);
			ImGui::Text("Player 1 Speed: %.3f", playerSpeed);
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

				XMFLOAT3 cPos = Camera::GetPosition(0);
				XMFLOAT3 cTar = Camera::GetTarget(0);
				ImGui::Text("Player 1 CamPos: (%.3f, %.3f, %.3f)", cPos.x, cPos.y, cPos.z);
				ImGui::Text("Player 1 CamTar: (%.3f, %.3f, %.3f)", cTar.x, cTar.y, cTar.z);
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
				ImGui::Text("Player 2 State: %s", stateName);
				ImGui::Text("Player 2 AnimFrameUp: %i", animFrameUp);
				ImGui::Text("Player 2 AnimFrameDo: %i", animFrameDown);

				XMFLOAT3 cPos = Camera::GetPosition(1);
				XMFLOAT3 cTar = Camera::GetTarget(1);
				ImGui::Text("Player 2 CamPos: (%.3f, %.3f, %.3f)", cPos.x, cPos.y, cPos.z);
				ImGui::Text("Player 2 CamTar: (%.3f, %.3f, %.3f)", cTar.x, cTar.y, cTar.z);
			}

		}
		break;

		case SelectedType::Enemy:
		{
			// 選択されたエネミーが存在しない場合は処理を終了
			if (enemies.empty() || !enemies[selectedIndex]) {
				selectedType = SelectedType::None;
				selectedIndex = -1;
				break;
			}

			// 選択中のエネミーを取得
			const auto& enemy = enemies[selectedIndex];
			if (!enemy && enemy->IsDead()) {
				selectedType = SelectedType::None;
				selectedIndex = -1;
				break;
			}

			int modelHandle = enemy->GetModelHandle();
			XMFLOAT3 currentPosition = enemy->GetPosition();
			XMFLOAT3 currentRotation = enemy->GetRotate();

			//PositionとRotationを表示
			ImGui::SliderFloat("Position X", &currentPosition.x, 20.0f, 80.0f, "%.2f");
			ImGui::SliderFloat("Position Y", &currentPosition.y, -10.0f, 10.0f, "%.2f");
			ImGui::SliderFloat("Position Z", &currentPosition.z, 20.0f, 80.0f, "%.2f");
			ImGui::SliderFloat3("Rotation", reinterpret_cast<float*>(&currentRotation), -180.0f, 180.0f, "%.2f");
			ImGui::Spacing();

			// 更新されたPositionとRotationをエネミーに反映
			enemy->SetPosition(currentPosition);
			enemy->SetRotate(currentRotation);

			// ボーンのコライダー情報を取得
			std::vector<BoneColliderData> boneColliders = *Model::GetBoneColliderData(modelHandle);

			ImGui::Separator();
			ImGui::Spacing();

			// ボーンの情報を折りたたみ可能に表示
			for (size_t i = 0; i < boneColliders.size(); ++i) {
				BoneColliderData& colliderData = boneColliders[i];

				// ボーン名を最小表示（折りたたみ可能）
				bool isOpen = ImGui::CollapsingHeader(colliderData.boneName.c_str());

				if (isOpen) {
					// 詳細情報を表示
					ImGui::PushID(static_cast<int>(i)); // 複数コライダーを識別するためのID
					ImGui::Text("Bone: %s", colliderData.boneName.c_str());

					// OffsetPosition の編集
					bool positionChanged = false;
					positionChanged |= ImGui::SliderFloat3("Offset Position", reinterpret_cast<float*>(&colliderData.offsetPosition), -5.0f, 5.0f);

					// OffsetRotationMatrix（回転のオフセット）をラジアン変換して編集
					XMFLOAT3 offsetRotationEuler = colliderData.offsetRotation;
					bool rotationChanged = false;
					rotationChanged |= ImGui::SliderFloat3("Offset Rotation", reinterpret_cast<float*>(&offsetRotationEuler), -180.0f, 180.0f);

					// 変更があった場合に値をセット
					if (positionChanged || rotationChanged) {
						Model::SetAttachColliderToBone(modelHandle, colliderData.pCollider, colliderData.offsetPosition, offsetRotationEuler);
					}

					// Collider自体の情報
					if (colliderData.pCollider) {
						float radius = colliderData.pCollider->size_.x;
						ImGui::SliderFloat("Radius", &radius, 0.0f, 10.0f, "%.2f");
						colliderData.pCollider->size_ = XMFLOAT3(radius, radius, radius);

						// Height のスライダー
						ImGui::SliderFloat("Height", &colliderData.pCollider->height_, 0.0f, 10.0f, "%.2f");
					
						//表示非表示
						int isDrawCollider = colliderData.pCollider->isDraw_;
						ImGui::SliderInt("Draw", &isDrawCollider, 0, 1, "%.2d");
						colliderData.pCollider->isDraw_ = isDrawCollider;

					}

					ImGui::Separator();
					ImGui::Spacing();
					ImGui::PopID();
				}
			}

			//セーブボタン
			if (ImGui::Button("CreateCollisionMap")) {
				BoneAttachColliderData BoneAttachDataGame[14];
				for (int i = 0; i < 14; i++) {
					boneColliders;
					BoneAttachDataGame[i].boneName = boneColliders[i].boneName;
					BoneAttachDataGame[i].height = boneColliders[i].pCollider->height_;
					BoneAttachDataGame[i].position = boneColliders[i].offsetPosition;
					BoneAttachDataGame[i].radius = boneColliders[i].pCollider->size_.x;
					BoneAttachDataGame[i].rotation = boneColliders[i].offsetRotation;
				}

				SaveBoneAttachData("Json/TestBossAttachCollider", BoneAttachDataGame, 14);
			}
		}
		break;

		case SelectedType::None:
			ImGui::Text("No selection.");
			break;
		}

		StageEditor::DrawStageEditor();

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
		Direct3D::SetViewPort(drawIndex_);
		Direct3D::SetViewOne();
		Camera::SetOneProjectionMatrix(Camera::GetFovAngleParcent(drawIndex_));
		
		if (isShadowDraw_) ShadowDraw();
		
		Direct3D::BeginDraw();
		Camera::Update(drawIndex_);
		pRootObject_->DrawSub();
		EFFEKSEERLIB::gEfk->Draw(drawIndex_);
		GameManager::IndividualDraw(drawIndex_);
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
			EFFEKSEERLIB::gEfk->Draw(i);
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
