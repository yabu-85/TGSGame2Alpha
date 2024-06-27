#pragma once
#include <vector>
#include <string>

class CollisionMap;
class Node;
struct StageModelData;

namespace StageEditor {
	//Modelーーーーーーーーーーーーーーーーーーーーーーーーーー
    std::vector<StageModelData> LoadFileStage(const std::string& fileName);
	void SaveFileStage(const std::vector<StageModelData>& stage, const std::string& fileName);
	void DrawStageEditor();
	
	//経路探索のノードーーーーーーーーーーーーーーーーーーー
	std::vector<Node*> LoadFileNode(const std::string& fileName);
	void SaveFileNode(std::vector<Node*>& nodes, const std::string& fileName);
	void DrawNodeEditor();

}