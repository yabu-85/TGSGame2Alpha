#pragma once
#include <DirectXMath.h>
#include <vector>
using namespace DirectX;

struct Edge {
    int connectId;  //Œq‚ª‚Á‚Ä‚éæ‚ÌID
    float cost;
    Edge() : connectId(-1), cost(-1) {}
    Edge(int t, float c) : connectId(t), cost(c) {}
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
        return std::numeric_limits<float>::infinity(); // ƒGƒbƒW‚ªŒ©‚Â‚©‚ç‚È‚¢ê‡
    }

    int GetId()& { return id_; }
    XMFLOAT3& GetPosition() { return position_; }
    std::vector<Edge>& GetEdges() { return edges_; }

    void SetId(int id)& { id_ = id; }
    void SetPosition(XMFLOAT3 pos) { position_ = pos; }
    void SetEdges(std::vector<Edge> edge) { edges_ = edge; }

};