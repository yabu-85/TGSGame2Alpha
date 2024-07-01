#pragma once
#include "../Engine/GameObject.h"
#include <vector>

class SelectScene : public GameObject
{
public:
	SelectScene(GameObject* parent);
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Release() override;
};