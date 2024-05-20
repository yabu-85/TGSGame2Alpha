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
	XMFLOAT4 pos = Light::GetPosition(0);
	if (Input::IsKey(DIK_LEFTARROW)) pos.x += 0.1f;
	if (Input::IsKey(DIK_RIGHTARROW)) pos.x -= 0.1f;
	if (Input::IsKey(DIK_UPARROW)) pos.z += 0.1f;
	if (Input::IsKey(DIK_DOWNARROW)) pos.z -= 0.1f;
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
