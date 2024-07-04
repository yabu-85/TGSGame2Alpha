#include "SceneBase.h"
#include "../Screen/Screen.h"
#include "../Other/GameManager.h"

SceneBase::SceneBase(GameObject* parent, std::string name)
	: GameObject(parent, name)
{
	GameManager::SetScene(this);
}

SceneBase::~SceneBase()
{
	for (auto u : pScreenList_)
	{
		delete u;
	}
	pScreenList_.clear();

}

void SceneBase::Initialize()
{
}

void SceneBase::Update()
{
	if (!pScreenList_.empty()) pScreenList_.back()->Update();

	// UI��delete
	for (auto iter = pScreenList_.begin(); iter != pScreenList_.end();) {
		if ((*iter)->GetUIState() == SCREEN_STATE::ENDDRAW)
		{
			delete* iter;
			iter = pScreenList_.erase(iter);
		}
		else
		{
			++iter;
		}
	}

}

void SceneBase::Draw()
{
}

void SceneBase::Release()
{
}


void SceneBase::AddScreen(Screen* screen)
{
	pScreenList_.push_back(screen);
}

void SceneBase::DeleteScreen(Screen* screen)
{
	for (auto iter = pScreenList_.begin(); iter != pScreenList_.end();) {
		if ((*iter) == screen)
		{
			delete* iter;
			iter = pScreenList_.erase(iter);
			return;
		}
	}
}

void SceneBase::AllDeleteScreen()
{
	for (auto s : pScreenList_) {
		{
			delete s;

		}
	}

	pScreenList_.clear();
}

void SceneBase::DeleteUI(UIBase* ui)
{
	for (auto iter = pScreenList_.begin(); iter != pScreenList_.end();) {
		if ((*iter)->DeleteUI(ui)) break;
	}
}

void SceneBase::CommonUIDraw()
{
	//���X�g�Ɉ�ł�����Ȃ�őO��\��
	if (!pScreenList_.empty()) pScreenList_.back()->Draw();

}

void SceneBase::IndividualUIDraw(int index)
{
}