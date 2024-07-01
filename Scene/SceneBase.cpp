#include "SceneBase.h"
#include "../Screen/Screen.h"

SceneBase::SceneBase(GameObject* parent, std::string name)
	: GameObject(parent, name)
{
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

	// UIのdelete
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
	//リストに一つでもあるなら最前列表示
	if (!pScreenList_.empty()) pScreenList_.back()->Draw();

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