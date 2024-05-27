#include "RouteSearch.h"
#include "Node.h"
#include "../Engine/Global.h"
#include "../Engine/Model.h"
#include "../Engine/StageEditor.h"

#include <unordered_map>
#include <cmath>
#include <limits>
#include <algorithm>
#include <queue>

namespace RouteSearch {
    int nodeHandle = -1;
    int edgeHandle = -1;
    std::vector<Node*> nodes;

    void InitializeList()
    {
        nodes = StageEditor::LoadFileNode("TestStageNode.json");
        nodeHandle = Model::Load("DebugCollision/SphereCollider.fbx");
        edgeHandle = Model::Load("DebugCollision/BoxCollider.fbx");
        assert(nodeHandle >= 0);
        assert(edgeHandle >= 0);
    }

    std::vector<Node*>& GetNodeList()
    {
        return nodes;
    }

    void NodeModelDraw()
    {
        for (Node* node : nodes) {
            Transform t = Transform();
            t.scale_ = XMFLOAT3(0.3f, 0.3f, 0.3f);
            t.position_ = node->GetPosition();
            Model::SetTransform(nodeHandle, t);
            Model::Draw(nodeHandle);

            for (Edge edge : node->GetEdges()) {
                //ÉGÉâÅ[èàóù
                if (edge.connectId <= -1) continue;

                XMFLOAT3 vec = Float3Sub(nodes[edge.connectId]->GetPosition(), node->GetPosition());
                vec = Float3Multiply(vec, 0.2f);
                XMFLOAT3 pos = t.position_;
                for (int i = 0; i < 5; i++) {
                    pos = Float3Add(pos, vec);
                    t.scale_ = XMFLOAT3(0.1f, 0.1f, 0.1f);
                    t.position_ = pos;
                    Model::SetTransform(edgeHandle, t);
                    Model::Draw(edgeHandle);
                }
            }
        }
    }

    float distance(Node& a, Node& b)
    {
        XMFLOAT3 aP = a.GetPosition();
        XMFLOAT3 bP = b.GetPosition();
        return CalculationDistance(Float3Sub(aP, bP));
    }

    std::vector<XMFLOAT3> AStar(const std::vector<Node*>& nodes, int start_id, int goal_id)
    {
        std::unordered_map<int, float> g_score;
        std::unordered_map<int, float> f_score;
        std::unordered_map<int, int> came_from;
        std::priority_queue<std::pair<float, int>, std::vector<std::pair<float, int>>, std::greater<>> open_set;

        for (const auto& node : nodes) {
            g_score[node->GetId()] = std::numeric_limits<float>::infinity();
            f_score[node->GetId()] = std::numeric_limits<float>::infinity();
        }

        g_score[start_id] = 0;
        f_score[start_id] = distance(*nodes[start_id], *nodes[goal_id]);

        open_set.emplace(f_score[start_id], start_id);

        while (!open_set.empty()) {
            int current = open_set.top().second;
            open_set.pop();

            if (current == goal_id) {
                std::vector<XMFLOAT3> path;
                while (came_from.find(current) != came_from.end()) {
                    path.push_back(nodes[current]->GetPosition());
                    current = came_from[current];
                }
                path.push_back(nodes[start_id]->GetPosition());
                std::reverse(path.begin(), path.end());
                return path;
            }

            std::vector<Edge> edgeList = nodes[current]->GetEdges();
            for (const auto& edge : edgeList) {
                int neighbor_id = edge.connectId;
                float tentative_g_score = g_score[current] + nodes[current]->GetCost(*nodes[neighbor_id]);
                if (tentative_g_score < g_score[neighbor_id]) {
                    came_from[neighbor_id] = current;
                    g_score[neighbor_id] = tentative_g_score;
                    f_score[neighbor_id] = g_score[neighbor_id] + distance(*nodes[neighbor_id], *nodes[goal_id]);
                    open_set.emplace(f_score[neighbor_id], neighbor_id);
                }
            }
        }

        return {}; // åoòHÇ™å©Ç¬Ç©ÇÁÇ»Ç©Ç¡ÇΩèÍçá
    }
}