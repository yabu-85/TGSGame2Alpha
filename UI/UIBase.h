#pragma once
#include <DirectXMath.h>
#include <string>
#include <functional>
#include "../Engine/Transform.h"

using namespace DirectX;

class UIBase
{
protected:
	bool isBound_;						//範囲の内側にいるかどうか
	bool isSelect_;						//コントローラー用選択しているか
	bool selectPlus_;					//アニメーション計算用
	bool screenSelectPossible_;			//スクリーン画面でセレクトを変動させていいか

	int hButtonPict_[2];				//UI本体の画像ハンドル
	int hImagePict_;					//表示する画像のハンドル
	int alpha_;							//透明度
	float selectAnim_;					//選択中のアニメーション０～１

	Transform frameTransform_;			//UI自体のTransform
	Transform imageTransform_;			//表示するImageとかのTransform
	std::function<void()> onClick_;		//UIを押したら呼ぶ関数

	//選択中のselectAnimの計算
	void SelectAnimUpdate();			

public:
	UIBase(XMFLOAT2 pos, XMFLOAT2 size, std::function<void()> onClick, XMFLOAT2 tsize);
	virtual ~UIBase();
	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual void Initialize(std::string name) = 0;
	virtual bool IsWithinBound() = 0;
	virtual void SelectUpdate() = 0;
	virtual void OnClick();

	XMFLOAT3 GetPosition() { return frameTransform_.position_; }

	bool GetBound() { return isBound_; }
	bool GetSelect() { return isSelect_; }
	bool GetSelectPossible() { return screenSelectPossible_; }

	void SetBound(bool b) { isBound_ = b; }
	void SetSelect(bool b) { isSelect_ = b; }

	/// <summary>
	/// UIの生成
	/// Sliderの名前指定は現在実装してないから、何を入れてもいい
	/// Buttonは読み込む文字画像の名前にすること
	/// </summary>
	template<class T>
	inline T* UIInstantiate(std::string name, XMFLOAT2 pos, XMFLOAT2 size, std::function<void()> onClick)
	{
		return UIInstantiate<T>(name, pos, size, XMFLOAT2(1.0f, 1.0f), onClick);
	}
	template<class T>
	inline T* UIInstantiate(std::string name, XMFLOAT2 pos, XMFLOAT2 size, XMFLOAT2 tsize, std::function<void()> onClick)
	{
		T* ui = new T(pos, size, onClick, tsize);
		ui->Initialize(name);
		return ui;
	}

};

