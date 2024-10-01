#include "sceneManager.h"
#include "../Scene/TitleScene.h"
#include "../Scene/PlayScene.h"

#include "Model.h"
#include "Image.h"
#include "Audio.h"
#include "../Other/GameManager.h"

//�R���X�g���N�^
SceneManager::SceneManager(GameObject * parent)
	: GameObject(parent, "SceneManager"), currentSceneID_(SCENE_ID_TITLE), nextSceneID_(SCENE_ID_TITLE)
{
}

//������
void SceneManager::Initialize()
{
	//�ŏ��̃V�[��������
#if 0
	currentSceneID_ = SCENE_ID_TITLE;
	nextSceneID_ = currentSceneID_;
	Instantiate<TitleScene>(this);
#else
	currentSceneID_ = SCENE_ID_PLAY;
	nextSceneID_ = currentSceneID_;
	Instantiate<PlayScene>(this);
#endif
	
}

//�X�V
void SceneManager::Update()
{
	//���̃V�[�������݂̃V�[���ƈႤ�@���@�V�[����؂�ւ��Ȃ���΂Ȃ�Ȃ�
	if (currentSceneID_ != nextSceneID_)
	{
		//���̃V�[���̃I�u�W�F�N�g��S�폜
		KillAllChildren();

		//���[�h�����f�[�^��S�폜
		Audio::Release();
		Model::AllRelease();
		Image::AllRelease();

		//�V�[���`�F���W�̏���
		GameManager::SceneChange();

		//���̃V�[�����쐬
		switch (nextSceneID_)
		{
		case SCENE_ID_TITLE: Instantiate<TitleScene>(this); break;
		case SCENE_ID_PLAY: Instantiate<PlayScene>(this); break;

		}
		Audio::Initialize();
		currentSceneID_ = nextSceneID_;
	}
}

//�`��
void SceneManager::Draw()
{
}

//�J��
void SceneManager::Release()
{
}

//�V�[���؂�ւ��i���ۂɐ؂�ւ��̂͂��̎��̃t���[���j
void SceneManager::ChangeScene(SCENE_ID next)
{
	nextSceneID_ = next;
}