#pragma once

class Screen;
class UIBase;

namespace ScreenManager
{
	//Screen追加
	void AddScreen(Screen* screen);
	
	//Screen削除
	void DeleteScreen(Screen* screen);
	
	//Screen初期化
	void AllDeleteScreen();

	//ScreenリストのUIListに検索かけてUI削除
	void DeleteUI(UIBase* ui);

	//Screenリストの更新
	void Update();

	//Screenリストの描画
	void Draw();

};