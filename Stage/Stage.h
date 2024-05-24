#pragma once
#include "../Engine/GameObject.h"

class Stage : public GameObject
{
	int hModel_;                //���f���ԍ�
public:
	Stage(GameObject* parent);
	~Stage();
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Release() override;
};