#include "GameManager.h"
#include "../Engine/Direct3D.h"
#include "../Engine/SceneManager.h"
#include "../Engine/RootObject.h"
#include "../Engine/Light.h"
#include "../Engine/ImGui/imgui.h"
#include "../Engine/ImGui/imgui_impl_dx11.h"
#include "../Engine/ImGui/imgui_impl_win32.h"

#include "../Stage/StageEditor.h"
#include "../Stage/CollisionMap.h"
#include "../Player/Player.h"
#include "../Other/InputManager.h"
#include "../UI/DamageUI.h"
#include "../Scene/SceneBase.h"

namespace GameManager {
	XMFLOAT3 SHADOW_CAMERA_TARGET = XMFLOAT3(50.0f, 0.0f, 50.0f);

	bool isOnePlayer_ = true;
	bool isPCCtrl_ = true;
	int drawIndex_ = 0;

	GameObject* pRootObject_ = nullptr;
	CollisionMap* pCollisionMap_ = nullptr;
	Player* pPlayer_[2] = { nullptr, nullptr };
	SceneBase* pScene_ = nullptr;

	void Initialize()
	{
		InputManager::Initialize();

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

	void Update()
	{
		DamageUI::Update();

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

	//人数関係
	void SetOnePlayer() { isOnePlayer_ = true; }
	void SetTwoPlayer() { isOnePlayer_ = false; }
	bool IsOnePlayer() { return isOnePlayer_; }

	int GetDrawIndex() { return drawIndex_; }

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
			pScene_->CommonUIDraw();
		}
	}

	void IndividualDraw(int index)
	{
		if (Direct3D::GetCurrentShader() == Direct3D::SHADER_3D) {
			pScene_->IndividualUIDraw(index);
		}

		DamageUI::Draw(index);
	}

	void ImGuiDraw()
	{
		//ImGuiの更新処理
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		StageEditor::DrawStageEditor();
		StageEditor::DrawNodeEditor();

		ImGui::Begin("Hello");//ImGuiの処理を開始
		{
			XMFLOAT4 position = Light::GetPosition(0);
			XMFLOAT4 target = Light::GetTarget(0);
			ImGui::Text("Position: (%.2f, %.2f, %.2f)", position.x, position.y, position.z);
			ImGui::Text("Target: (%.2f, %.2f, %.2f)", target.x, target.y, target.z);
			ImGui::Separator();
			ImGui::Text("Player: (%.2f, %.2f, %.2f)", Direct3D::PlayerPosition.x, Direct3D::PlayerPosition.y, Direct3D::PlayerPosition.z);
			ImGui::Text("Enemy : (%.2f, %.2f, %.2f)", Direct3D::EnemyPosition.x, Direct3D::EnemyPosition.y, Direct3D::EnemyPosition.z);
			ImGui::Separator();
			ImGui::SliderFloat("Player Speed", &Direct3D::playerSpeed, 0.0f, 1.0f);

			if (Direct3D::playerFaly) ImGui::Text("Player Faly   : true");
			else ImGui::Text("Player Faly  : false");

			if (Direct3D::playerClimb) ImGui::Text("Player Climb : true");
			else ImGui::Text("Player Climb : false");
		}
		ImGui::End();//ImGuiの処理を終了
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}

	void ShadoDraw()
	{	
		XMFLOAT3 pos = Camera::GetPosition(0);
		XMFLOAT3 tar = Camera::GetTarget(0);
		XMFLOAT4 lightPos = Light::GetPosition(0);
		Camera::SetPosition(XMFLOAT3(lightPos.x, lightPos.y, lightPos.z), 0);
		Camera::SetTarget(SHADOW_CAMERA_TARGET, 0);
		Camera::Update(0);
		Direct3D::lightViewMatrix = Camera::GetViewMatrix();

		Direct3D::BeginDraw();
		pRootObject_->DrawSub();
		Direct3D::EndDraw();
		Camera::SetPosition(pos, 0);
		Camera::SetTarget(tar, 0);
	}

	void OnePlayerDraw()
	{
	//	ShadoDraw();

		Direct3D::BeginDraw2();
		Camera::Update(0);
		pRootObject_->DrawSub();
		EFFEKSEERLIB::gEfk->Draw();
		GameManager::IndividualDraw(0);
		GameManager::CommonDraw();

		ImGuiDraw();
		Direct3D::EndDraw();
	}

	void TwoPlayerDraw()
	{
		Camera::SetTwoProjectionMatrix();
		ShadoDraw();

		Direct3D::BeginDraw2();
		Direct3D::SetViewTwo();
		for (int i = 0; i < 2; i++) {
			drawIndex_ = i;
			Direct3D::SetViewPort(i);
			Camera::Update(i);
			pRootObject_->DrawSub();
			EFFEKSEERLIB::gEfk->Draw();
			GameManager::IndividualDraw(i);
		}

		Direct3D::SetViewOne();
		Camera::SetOneProjectionMatrix();
		GameManager::CommonDraw();

		ImGuiDraw();
		Direct3D::EndDraw();
	}

}
