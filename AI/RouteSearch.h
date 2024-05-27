#pragma once
#include <vector>
#include <DirectXMath.h>

using namespace DirectX;
class Node;

namespace RouteSearch {
    void InitializeList();
    std::vector<Node*>& GetNodeList();
    
    void NodeModelDraw();

    //�m�[�h�Ԃ̋������v�Z����֐�
    float distance(Node& a, Node& b);

    //A*�A���S���Y���ɂ��o�H�T��
    std::vector<XMFLOAT3> AStar(const std::vector<Node*>& nodes, int start_id, int goal_id);

}