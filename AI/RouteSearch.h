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

    void Initialize();
    void Release();

    std::vector<Node*>& GetNodeList();
    
    void NodeModelDraw();

    //�m�[�h�Ԃ̋������v�Z����֐�
    float distance(Node& a, Node& b);

    //�|�W�V�����ɑ΂��čœK�ȃm�[�h�ԍ����v�Z
    //������Ȃ������E���������ꍇ�F-1���A���Ă���
    int GetNodeToPosition(const XMFLOAT3& pos);

    //A*�A���S���Y���ɂ��o�H�T��
    std::vector<RouteData> AStar(int goal_id, int start_id);
    std::vector<RouteData> AStar(int goal_id, const XMFLOAT3& s_pos);
    std::vector<RouteData> AStar(const XMFLOAT3& t_pos, const XMFLOAT3& s_pos);

    //�p�X�̃X���[�W���O
    void PathSmoothing(std::vector<RouteData>& path);

}