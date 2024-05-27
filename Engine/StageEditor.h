#pragma once
#include <vector>
#include <string>

class CollisionMap;
struct StageModelData;

namespace StageEditor {
    std::vector<StageModelData> LoadFileStage(const std::string& fileName);
	void SaveFileStage(const std::vector<StageModelData>& stage, const std::string& fileName);

	void SetCollisionMap(CollisionMap* map);
	void DrawStageEditor();
	
}