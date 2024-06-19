#pragma once
#include "Image.h"

class Text
{
	int hPict_;						//画像ハンドル
	int	alpha;						//アルファ

	unsigned int width_, height_;	//1文字分の幅と高さ
	unsigned int rowLength_;		
	char fileName_[MAX_PATH];

public:
	Text();
	~Text();

	//初期化（デフォルトの設定）
	//戻値：成功／失敗
	HRESULT Initialize();

	//初期化（オリジナルの画像を使いたい時）
	//引数：fileName	画像ファイル名
	//引数：charWidth	画像の１文字の幅（ピクセル）
	//引数：charHeight	画像の１文字の高さ（ピクセル）
	//引数：rowLength	画像の中に横に何文字並んでいるか
	//戻値：成功／失敗
	HRESULT Initialize(const char* fileName, const unsigned int charWidth, const unsigned int charHeight, const unsigned int rowLength);

	//描画（文字列）
	//引数：x, y	表示位置（左上）
	//引数：str		表示したい文字列
	void Draw(int x, int y, const char* str);

	//描画（整数値）
	//引数：x, y	表示位置（左上）
	//引数：value	表示したい値
	void Draw(int x, int y, int value);

	//アルファ値を設定（不透明度）　0～255で指定
	void SetAlpha(int _alpha);

	//解放
	void Release();
};

