#pragma once
#include "../Engine/GameObject.h"
#include <vector>

class TestScene : public GameObject
{
public:
	TestScene(GameObject* parent);
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Release() override;
};