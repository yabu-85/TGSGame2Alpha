#pragma once
#include "../Engine/GameObject.h"
#include <vector>

class Screen;
class UIBase;

class SceneBase : public GameObject
{
protected:
    std::vector<Screen*> pScreenList_;

public:
	SceneBase(GameObject* parent, std::string name);
    virtual ~SceneBase();
    virtual void Initialize() override;
    virtual void Update() override;
    virtual void Draw() override;
    virtual void Release() override;

	//Screen追加
	void AddScreen(Screen* screen);

	//Screen削除
	void DeleteScreen(Screen* screen);

	//Screen初期化
	void AllDeleteScreen();

	//ScreenリストのUIListに検索かけてUI削除
	void DeleteUI(UIBase* ui);

	//登録された共通UIの描画
	virtual void CommonUIDraw();

	//登録された個別UIの描画
	virtual void IndividualUIDraw();
	
};