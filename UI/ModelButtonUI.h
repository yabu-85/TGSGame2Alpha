#pragma once
#include "../Engine/Transform.h"
#include "UIBase.h"

//ImageTransform��

/// <summary>
/// ���f����\������{�^��
/// </summary>
class ModelButtonUI : public UIBase
{
	int hModel_;				//�\�����郂�f��

	XMFLOAT2 widePos_;			//0�`1980�Ƃ��̍��W
	XMFLOAT2 frameHalfSize_;	//�����̃T�C�Y

public:
	ModelButtonUI(XMFLOAT2 pos, XMFLOAT2 size, std::function<void()> onClick, XMFLOAT2 tsize);
	~ModelButtonUI() override;
	void Update() override;
	void Draw() override;
	void Initialize(std::string name) override;
	bool IsWithinBound() override;
	void SelectUpdate() override;

};

