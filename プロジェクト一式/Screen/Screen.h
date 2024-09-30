#pragma once
#include <vector>
#include <string>
#include <DirectXMath.h>
#include<functional>

using namespace DirectX;
class UIBase;

enum SCREEN_STATE {
	DRAW = 0,	//�`�撆
	ENDDRAW,	//�`��I���iKill�����j
};

class Screen
{
	int selectCoolTime_;			//�R���g���[���[�p�I���N�[���_�E��
	int latestSelectUIIndex_;		//�Q�[���p�b�h�g�p���̑I�����Ă�UI�ԍ�

	static bool latestPCCtrl_;		//�ŐV�̃A�N�V������PC���i�S�X�N���[������

protected:
	std::vector<UIBase*> uiList_;	//�o�^���ꂽUI�ꗗ
	SCREEN_STATE state_;			//�`��X�e�[�g

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
	void SetUIState(SCREEN_STATE state) { state_ = state; }
};