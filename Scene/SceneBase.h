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

	//Screen�ǉ�
	void AddScreen(Screen* screen);

	//Screen�폜
	void DeleteScreen(Screen* screen);

	//Screen������
	void AllDeleteScreen();

	//Screen���X�g��UIList�Ɍ���������UI�폜
	void DeleteUI(UIBase* ui);

	//�o�^���ꂽ����UI�̕`��
	virtual void CommonUIDraw();

	//�o�^���ꂽ��UI�̕`��
	virtual void IndividualUIDraw(int index);
	
};