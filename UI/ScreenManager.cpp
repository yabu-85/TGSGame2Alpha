#include "ScreenManager.h"
#include "Screen.h"
#include <vector>

namespace ScreenManager {
	std::vector<Screen*> pScreenList_;

	void AddScreen(Screen* screen)
	{
		pScreenList_.push_back(screen);
	}

	void DeleteScreen(Screen* screen)
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

	void ResetScreenList()
	{
		for (auto iter = pScreenList_.begin(); iter != pScreenList_.end();) {
			if ((*iter)->GetUIState() == Screen::UI_STATE::ENDDRAW)
			{
				delete* iter;
			}
		}

		pScreenList_.clear();
	}

	void DeleteUI(UIBase* ui)
	{
		for (auto iter = pScreenList_.begin(); iter != pScreenList_.end();) {
			if ((*iter)->DeleteUI(ui)) break;
		}
	}

	void Update()
	{
		if (!pScreenList_.empty()) pScreenList_.back()->Update();

	}

	void Draw()
	{
		//リストに一つでもあるなら最前列表示
		if (!pScreenList_.empty()) pScreenList_.back()->Draw();
	
	}

}