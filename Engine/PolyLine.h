#pragma once
#include <list>
#include "Direct3D.h"
#include "Texture.h"
#include <DirectXMath.h>
#include <string>
#include "BillBoard.h"

using namespace DirectX;
using namespace std;

class PolyLine
{
protected:
	int length_;			//長さ
	float width_;			//太さ
	float alpha_;			//透明度

	bool moveAlpha_;		//徐々に透明にしておく
	bool first_;			//最初のデータか
	bool allClearReset_;	//すべて消したらClearを取り消すか

	ID3D11Buffer* pVertexBuffer_;	//頂点バッファ
	ID3D11Buffer* pConstantBuffer_;	//コンスタントバッファ
	Texture* pTexture_;	            //画像

	list<XMFLOAT3> positions_;	    //過去length_回分の位置

	virtual void CalcPoly();
public:
	PolyLine();
	void Draw();
	virtual void Release();

	//引数：fileName	画像ファイル名
	HRESULT Load(std::string fileName);
	
	//全てのポジションをリセット
	virtual void ResetPosition();

	//一番後ろのデータを消す
	virtual void ClearLastPosition();

	//一番最初のデータを消す
	virtual void ClearFirstPosition();

	//現在の位置を記憶させる
	//引数：pos	現在の位置
	void AddPosition(XMFLOAT3 pos);

	//徐々に透明になるように設定
	void SetMoveAlphaFlag() { moveAlpha_ = true; }
	void SetLength(int leng) { length_ = leng; }
	void SetWidth(float width) { width_ = width; }
};

class DoublePolyLine : public PolyLine {
	list<XMFLOAT3> positionsSub_;	//座標を指定したバージョンで使用

	void CalcPoly() override;
public:
	DoublePolyLine();
	void Release() override;

	void ResetPosition() override;

	//一番後ろのデータを消す
	void ClearLastPosition() override;
	
	//一番最初のデータを消す
	void ClearFirstPosition() override;

	//現在の位置を記憶させる
	//引数：pos1, pos2 記憶させる位置
	void AddPosition(XMFLOAT3 pos1, XMFLOAT3 pos2);

};
