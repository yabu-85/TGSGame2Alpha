#pragma once
#include <DirectXMath.h>
#include <vector>
using namespace DirectX;

enum EdgeType {
    NORMAL = 0,
    JUMP,
};

struct Edge {
    EdgeType type;
    int connectId;  //åqÇ™Ç¡ÇƒÇÈêÊÇÃID
    float cost;
    Edge(EdgeType t) : connectId(-1), cost(-1), type(t) {}
    Edge(int t, float c, EdgeType ty) : connectId(t), cost(c), type(ty) {}
};

class Node {
    int id_;
    XMFLOAT3 position_;
    std::vector<Edge> edges_;

public:
    Node() : id_(0), position_(XMFLOAT3()) {}
    Node(int id, XMFLOAT3 pos) : id_(id), position_(pos) {}
    
    virtual float GetCost(const Node& neighbor) const {
        for (const auto& edge : edges_) {
            if (edge.connectId == neighbor.id_) {
                return edge.cost;
            }
        }
        return std::numeric_limits<float>::infinity(); // ÉGÉbÉWÇ™å©Ç¬Ç©ÇÁÇ»Ç¢èÍçá
    }

    int GetId()& { return id_; }
    XMFLOAT3& GetPosition() { return position_; }
    std::vector<Edge>& GetEdges() { return edges_; }

    void SetId(int id)& { id_ = id; }
    void SetPosition(XMFLOAT3 pos) { position_ = pos; }
    void SetEdges(std::vector<Edge> edge) { edges_ = edge; }

};