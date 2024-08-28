#pragma once
#include "../Engine/Transform.h"
#include "UIBase.h"

//ImageTransformを

/// <summary>
/// モデルを表示するボタン
/// </summary>
class ModelButtonUI : public UIBase
{
	int hModel_;				//表示するモデル

	XMFLOAT2 widePos_;			//0～1980とかの座標
	XMFLOAT2 frameHalfSize_;	//半分のサイズ

public:
	ModelButtonUI(XMFLOAT2 pos, XMFLOAT2 size, std::function<void()> onClick, XMFLOAT2 tsize);
	~ModelButtonUI() override;
	void Update() override;
	void Draw() override;
	void Initialize(std::string name) override;
	bool IsWithinBound() override;
	void SelectUpdate() override;

};

