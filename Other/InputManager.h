#pragma once

namespace InputManager {

	//判定するタイプ
	enum TYPE {
		KEY = 0,
		MOUSE,
		SCROLL_UP,
		SCROLL_DOWN,
		TYPE_MAX,
	};

	//判定するコマンド
	enum COMMAND {
		MOVE_LEFT = 0,
		MOVE_RIGHT,
		MOVE_UP,
		MOVE_DOWN, 

		ACTION,
		JUMP,
		CMD_MAX,
	};

	//KeyConfigデータをデフォルトの状態に戻す
	void SetDefaultKeyConfig();

	//コマンドの変更
	void ChangeCmd(COMMAND cmd, TYPE type, int num);

	//押している間
	bool IsCmd(COMMAND cmd);

	//離したとき
	bool IsCmdUp(COMMAND cmd);

	//押したとき
	bool IsCmdDown(COMMAND cmd);
	
	//移動入力をしているか
	bool CmdWalk();

};