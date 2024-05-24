#pragma once
#include "../Engine/GameObject.h"

class Stage : public GameObject
{
	int hModel_;                //ƒ‚ƒfƒ‹”Ô†
public:
	Stage(GameObject* parent);
	~Stage();
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Release() override;
};