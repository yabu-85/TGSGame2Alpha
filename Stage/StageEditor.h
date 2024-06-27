#pragma once
#include <vector>
#include <string>

class CollisionMap;
class Node;
struct StageModelData;

namespace StageEditor {
	//Model�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[
    std::vector<StageModelData> LoadFileStage(const std::string& fileName);
	void SaveFileStage(const std::vector<StageModelData>& stage, const std::string& fileName);
	void DrawStageEditor();
	
	//�o�H�T���̃m�[�h�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[
	std::vector<Node*> LoadFileNode(const std::string& fileName);
	void SaveFileNode(std::vector<Node*>& nodes, const std::string& fileName);
	void DrawNodeEditor();

}