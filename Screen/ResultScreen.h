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
	/// 勝ったほうのプレイヤーIDをセット（０ or １）して画像もその番号をLoad
	/// 初期値は引き分けの設定だからその場合は呼ばない
	/// </summary>
	/// <param name="i"></param>
	void SetWinPlayer(int i);

};

