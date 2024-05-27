#pragma once
#include <vector>
#include <DirectXMath.h>

using namespace DirectX;
class Node;

namespace RouteSearch {
    void InitializeList();
    std::vector<Node*>& GetNodeList();
    
    void NodeModelDraw();

    //ƒm[ƒhŠÔ‚Ì‹——£‚ğŒvZ‚·‚éŠÖ”
    float distance(Node& a, Node& b);

    //A*ƒAƒ‹ƒSƒŠƒYƒ€‚É‚æ‚éŒo˜H’Tõ
    std::vector<XMFLOAT3> AStar(const std::vector<Node*>& nodes, int start_id, int goal_id);

}