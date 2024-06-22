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

    //ノード間の距離を計算する関数
    float distance(Node& a, Node& b);

    //ポジションに対して最適なノード番号を計算
    //見つからなかった・遠すぎた場合：-1が帰ってくる
    int GetNodeToPosition(const XMFLOAT3& pos);

    //A*アルゴリズムによる経路探索
    std::vector<RouteData> AStar(int goal_id, int start_id);
    std::vector<RouteData> AStar(int goal_id, const XMFLOAT3& s_pos);
    std::vector<RouteData> AStar(const XMFLOAT3& t_pos, const XMFLOAT3& s_pos);

    //パスのスムージング
    void PathSmoothing(std::vector<RouteData>& path);

}