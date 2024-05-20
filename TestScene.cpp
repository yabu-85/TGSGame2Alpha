#include "TestScene.h"
#include "Player/Player.h"
#include "Stage/Stage.h"

#include "Engine/Input.h"
#include "Engine/Light.h"

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
	if (Input::IsMouseButton(0)) pos.y += speed;
	if (Input::IsMouseButton(1)) pos.y -= speed;
	Light::SetPosition(0, pos);

}

//�`��
void TestScene::Draw()
{
	Light::Draw();

}

//�J��
void TestScene::Release()
{
}
