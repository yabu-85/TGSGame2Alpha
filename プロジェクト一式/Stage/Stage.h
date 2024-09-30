#pragma once
#include "../Engine/GameObject.h"

struct StageEnvironment {
	float gravity;				//
	float startRotateY[2];		//
	XMFLOAT3 startPosition[2];	//
	XMFLOAT3 lightPosition;		//

	StageEnvironment() : gravity(0.0f), startRotateY{ 0.0f, 0.0f }, startPosition { { 0, 0, 0 }, { 0,0,0 } }, lightPosition{ 0,0,0 } {};
};

class Stage : public GameObject
{
	StageEnvironment stageEnvironment_;

public:
	Stage(GameObject* parent);
	~Stage();
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Release() override;

	float GetGravity() const { return stageEnvironment_.gravity; }
	void LoadStageEnvironment(std::string fileName);
	void SetStageEnvironment(StageEnvironment& info) { stageEnvironment_ = info; }
	StageEnvironment& GetStageEnvironment() { return stageEnvironment_; }
};