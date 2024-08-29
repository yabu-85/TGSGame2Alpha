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
	int selectCoolTime_;			//�R���g���[���[�p�I���N�[���_�E��

protected:
	std::vector<UIBase*> uiList_;	//�o�^���ꂽUI�ꗗ
	SCREEN_STATE state_;			//

public:
	Screen();
	virtual ~Screen();
	virtual void Update();
	virtual void Draw();

	//UI�ǉ�
	void AddUI(UIBase* ui);
	
	//UI�폜
	bool DeleteUI(UIBase* ui);

	SCREEN_STATE GetUIState() { return state_; };
};