#pragma once
#include <string>
#include <vector>
#include "../Engine/Transform.h"

class CollisionMap;
struct StageModelData;

struct LoadStageModel
{
	std::string fileName;
	Transform transform;
};

namespace StageEditor {
    std::vector<StageModelData> LoadFileStage(const std::string& fileName);
	void SaveFileStage(const std::vector<LoadStageModel>& stage, const std::string& fileName);

	void SetCollisionMap(CollisionMap* map);
	void DrawStageEditor();
	
}