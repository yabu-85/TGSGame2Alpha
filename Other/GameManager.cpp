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
#include "../Json/JsonReader.h"

namespace GameManager {
	XMFLOAT3 SHADOW_CAMERA_TARGET = XMFLOAT3(50.0f, 0.0f, 50.0f);

	bool isImGuiDraw_ = true;
	bool isOnePlayer_ = true;
	bool isPCCtrl_ = false;
	int pcCtrlNumber_ = 0;
	int drawIndex_ = 0;

	GameObject* pRootObject_ = nullptr;
	CollisionMap* pCollisionMap_ = nullptr;
	Player* pPlayer_[2] = { nullptr, nullptr };
	SceneBase* pScene_ = nullptr;

	void Initialize()
	{
		InputManager::Initialize();

		//PlayerSetting�ǂݍ���
		JsonReader::Load("Json/PlayerSetting.json");
		auto& player1Section = JsonReader::GetSection("Player1");
		auto& player2Section = JsonReader::GetSection("Player2");
		int pcCtrl1 = (player1Section["pcCtrl"]);
		int pcCtrl2 = (player2Section["pcCtrl"]);

		if (pcCtrl1 || pcCtrl2) {
			isPCCtrl_ = true;
			if (pcCtrl1 == 1) pcCtrlNumber_ = 0;
			else if (pcCtrl2 == 1) pcCtrlNumber_ = 1;
		}

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

	int GetDrawIndex() { return drawIndex_; }
	
	//�l���֌W
	void SetOnePlayer() { isOnePlayer_ = true; }
	void SetTwoPlayer() { isOnePlayer_ = false; }
	bool IsOnePlayer() { return isOnePlayer_; }

	//PCCtrl�֌W
	void SetPCCtrlON() { isPCCtrl_ = true; }
	void SetPCCtrlOFF() { isPCCtrl_ = false; }
	bool IsPCCtrl() { return isPCCtrl_; }
	void SetPCCtrlNumber(int number) { pcCtrlNumber_ = number; }
	int GetPCCtrlNumber() { return pcCtrlNumber_; }

	//�A�N�Z�T
	GameObject* GetRootObject() { return pRootObject_; }
	void SetRootObject(GameObject* root) { pRootObject_ = root; }
	CollisionMap* GameManager::GetCollisionMap() { return pCollisionMap_; }
	void GameManager::SetCollisionMap(CollisionMap* map) { pCollisionMap_ = map; }
	Player* GameManager::GetPlayer(int index) { return pPlayer_[index]; }
	void GameManager::SetPlayer(Player* player, int index) { pPlayer_[index] = player; }
	SceneBase* GetScene() { return pScene_; }
	void SetScene(SceneBase* stage) { pScene_ = stage; }

	//GameManager�̒��Ŏg��
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
		//ImGui�̍X�V����
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		StageEditor::DrawStageEditor();
		StageEditor::DrawNodeEditor();

		ImGui::Begin("Hello");//ImGui�̏������J�n
		{
			XMFLOAT3 position = Camera::GetPosition(0);
			XMFLOAT3 target = Camera::GetTarget(0);

			ImGui::Text("Position: (%.2f, %.2f, %.2f)", position.x, position.y, position.z);
			ImGui::Text("Target: (%.2f, %.2f, %.2f)", target.x, target.y, target.z);
			ImGui::Text("PlayerCamX: (%.2f)", Direct3D::playerCamX);
			ImGui::Text("PlayerCamX: (%.2f)", Direct3D::playerCamY);
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
		ImGui::End();//ImGui�̏������I��
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

		Direct3D::BeginDraw();
		pRootObject_->DrawSub();
		Direct3D::EndDraw();
		Camera::SetPosition(pos, 0);
		Camera::SetTarget(tar, 0);
		Camera::Update(0);
	}

	void OnePlayerDraw()
	{
		Direct3D::SetViewPort(0);
		Direct3D::SetViewOne();
		Camera::SetOneProjectionMatrix();
		drawIndex_ = 0;
		ShadowDraw();
		
		Direct3D::BeginDraw2();
		Camera::Update(0);
		Camera::SetOneProjectionMatrix(); 
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
		Camera::SetTwoProjectionMatrix();
		ShadowDraw();

		Direct3D::BeginDraw2();
		for (int i = 0; i < 2; i++) {
			drawIndex_ = i;
			Direct3D::SetViewPort(i);
			Camera::Update(i);
			Camera::SetTwoProjectionMatrix(Camera::GetPeekFOVZoom(i));

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
