#pragma once
#include "Screen.h"

class ResultScreen : public Screen
{
	enum ResulT_IMAGE
	{
		PLAYER = 0,
		BACK,
		MAX,
	};

	int hPict_[MAX];
	int winPlayerID_;

public:
	ResultScreen();
	~ResultScreen() override;
	void Draw() override;

	/// <summary>
	/// �������ق��̃v���C���[ID���Z�b�g�i�O or �P�j���ĉ摜�����̔ԍ���Load
	/// �����l�͈��������̐ݒ肾���炻�̏ꍇ�͌Ă΂Ȃ�
	/// </summary>
	/// <param name="i"></param>
	void SetWinPlayer(int i);

};

