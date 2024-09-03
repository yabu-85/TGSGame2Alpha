#pragma once

namespace InputManager {

	//���肷��^�C�v(�}�E�X�X�N���[�����ꉞ����Ă���j
	enum TYPE {
		KEY = 0,		//�L�[�{�[�h
		MOUSE,			//�}�E�X
		CONTROLLER,		//�R���g���[���[�̃{�^��
		TRRIGERL,		//���g���K�[
		TRRIGERR,		//�E�g���K�[
		STICKL,			//���X�e�B�b�N
		STICKR,			//�E�X�e�B�b�N
		TYPE_MAX,
	};

	//���肷��R�}���h
	enum COMMAND {
		MOVE_LEFT = 0,
		MOVE_RIGHT,
		MOVE_UP,
		MOVE_DOWN, 

		ATTACK,
		AIM,
		RELOAD,
		ACTION,
		JUMP,

		SELECT,
		RETURN,
		PAUSE,

		CMD_MAX,
	};

	void Initialize();

	//KeyConfig�f�[�^���f�t�H���g�̏�Ԃɖ߂�
	void SetDefaultKeyConfig();

	//�R�}���h�̕ύX
	void ChangeCmd(COMMAND cmd, TYPE type, int num);

	//�����Ă����
	bool IsCmd(COMMAND cmd, int id);

	//�������Ƃ�
	bool IsCmdUp(COMMAND cmd, int id);

	//�������Ƃ�
	bool IsCmdDown(COMMAND cmd, int id);
	
	//�ړ����͂����Ă��邩
	bool CmdWalk(int id);

};