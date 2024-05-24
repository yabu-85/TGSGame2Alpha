#include "TestScene.h"
#include "Player/Player.h"
#include "Stage/Stage.h"
#include "Enemy/Enemy.h"

#include "Engine/Input.h"
#include "Engine/Light.h"
#include "Engine/Direct3D.h"

//�R���X�g���N�^
TestScene::TestScene(GameObject * parent)
	: GameObject(parent, "TestScene")
{
}

//������
void TestScene::Initialize()
{
	Instantiate<Player>(this);
	Instantiate<Stage>(this);
	//Instantiate<Enemy>(this);

}

//�X�V
void TestScene::Update()
{
	static const float speed = 0.3f;
	XMFLOAT4 pos = Light::GetPosition(0);
	if (Input::IsKey(DIK_LEFTARROW)) pos.x += speed;
	if (Input::IsKey(DIK_RIGHTARROW)) pos.x -= speed;
	if (Input::IsKey(DIK_UPARROW)) pos.z += speed;
	if (Input::IsKey(DIK_DOWNARROW)) pos.z -= speed;
	if (Input::IsKey(DIK_E)) pos.y += speed;
	if (Input::IsKey(DIK_Q)) pos.y -= speed;
	Light::SetPosition(0, pos);

	//2��ʂ̕\�����e�؂�ւ�
	if (Input::IsKeyDown(DIK_R)) Direct3D::isTwoWindowShadowDraw_ = !Direct3D::isTwoWindowShadowDraw_;

}

//�`��
void TestScene::Draw()
{
}

//�J��
void TestScene::Release()
{
}
