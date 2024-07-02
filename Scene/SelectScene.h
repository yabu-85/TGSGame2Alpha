#pragma once
#include "SceneBase.h"

class SelectScene : public SceneBase
{
public:
	SelectScene(GameObject* parent);
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Release() override;
};