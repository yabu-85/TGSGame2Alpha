#pragma once
#include <vector>
#include <DirectXMath.h>
#include "Node.h"

using namespace DirectX;
class Node;

struct RouteData {
    EdgeType type;
    XMFLOAT3 pos;
    RouteData() : type(EdgeType::NORMAL), pos(XMFLOAT3()) {}
};

namespace RouteSearch {

    void InitializeList();
    std::vector<Node*>& GetNodeList();
    
    void NodeModelDraw();

    //ƒm[ƒhŠÔ‚Ì‹——£‚ğŒvZ‚·‚éŠÖ”
    float distance(Node& a, Node& b);

    //A*ƒAƒ‹ƒSƒŠƒYƒ€‚É‚æ‚éŒo˜H’Tõ
    std::vector<RouteData> AStar(const std::vector<Node*>& nodes, int start_id, int goal_id);

}