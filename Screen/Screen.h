#pragma once
#include <vector>
#include <string>
#include <DirectXMath.h>
#include<functional>

using namespace DirectX;
class UIBase;

enum SCREEN_STATE {
	DRAW = 0,
	ENDDRAW,
};

class Screen
{
	int selectCoolTime_;			//コントローラー用選択クールダウン

protected:
	std::vector<UIBase*> uiList_;	//登録されたUI一覧
	SCREEN_STATE state_;			//

public:
	Screen();
	virtual ~Screen();
	virtual void Update();
	virtual void Draw();

	//UI追加
	void AddUI(UIBase* ui);
	
	//UI削除
	bool DeleteUI(UIBase* ui);

	SCREEN_STATE GetUIState() { return state_; };
};