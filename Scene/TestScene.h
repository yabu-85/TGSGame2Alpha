#pragma once
#include "SceneBase.h"

class TestScene : public SceneBase
{
public:
	TestScene(GameObject* parent);
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Release() override;
};