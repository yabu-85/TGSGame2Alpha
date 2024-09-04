#pragma once
#include <vector>
#include <string>
#include <DirectXMath.h>
#include<functional>

using namespace DirectX;
class UIBase;

enum SCREEN_STATE {
	DRAW = 0,	//描画中
	ENDDRAW,	//描画終了（Killされる）
};

class Screen
{
	int selectCoolTime_;			//コントローラー用選択クールダウン

protected:
	std::vector<UIBase*> uiList_;	//登録されたUI一覧
	SCREEN_STATE state_;			//描画ステート

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
	void SetUIState(SCREEN_STATE state) { state_ = state; }
};