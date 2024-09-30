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

	//Screen’Ç‰Á
	void AddScreen(Screen* screen);

	//Screeníœ
	void DeleteScreen(Screen* screen);

	//Screen‰Šú‰»
	void AllDeleteScreen();

	//ScreenƒŠƒXƒg‚ÌUIList‚ÉŒŸõ‚©‚¯‚ÄUIíœ
	void DeleteUI(UIBase* ui);

	//“o˜^‚³‚ê‚½‹¤’ÊUI‚Ì•`‰æ
	virtual void CommonUIDraw();

	//“o˜^‚³‚ê‚½ŒÂ•ÊUI‚Ì•`‰æ
	virtual void IndividualUIDraw(int index);
	
};