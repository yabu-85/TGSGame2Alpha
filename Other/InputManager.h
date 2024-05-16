#pragma once

namespace InputManager {

	//���肷��^�C�v
	enum TYPE {
		KEY = 0,
		MOUSE,
		SCROLL_UP,
		SCROLL_DOWN,
		TYPE_MAX,
	};

	//���肷��R�}���h
	enum COMMAND {
		MOVE_LEFT = 0,
		MOVE_RIGHT,
		MOVE_UP,
		MOVE_DOWN, 

		ACTION,
		JUMP,
		CMD_MAX,
	};

	//KeyConfig�f�[�^���f�t�H���g�̏�Ԃɖ߂�
	void SetDefaultKeyConfig();

	//�R�}���h�̕ύX
	void ChangeCmd(COMMAND cmd, TYPE type, int num);

	//�����Ă����
	bool IsCmd(COMMAND cmd);

	//�������Ƃ�
	bool IsCmdUp(COMMAND cmd);

	//�������Ƃ�
	bool IsCmdDown(COMMAND cmd);
	
	//�ړ����͂����Ă��邩
	bool CmdWalk();

};