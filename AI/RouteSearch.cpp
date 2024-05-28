#include "RouteSearch.h"
#include "Node.h"
#include "../Engine/Global.h"
#include "../Engine/Model.h"
#include "../Engine/StageEditor.h"
#include "../Engine/Text.h"
#include "../Engine/Camera.h"

#include <unordered_map>
#include <cmath>
#include <limits>
#include <algorithm>
#include <queue>

namespace RouteSearch {
    Text* pText = nullptr;
    int nodeHandle = -1;
    int edgeHandle = -1;
    std::vector<Node*> nodes;

    void InitializeList()
    {
        pText = new Text();
        pText->Initialize();

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
        for (int i = 0; i < nodes.size(); i++) {
            Node* node = nodes[i];
            XMFLOAT3 vec = Float3Sub(Camera::GetPosition(), node->GetPosition());
            float dist = CalculationDistance(vec);
            if (dist > 30.0f) continue;

            Transform t = Transform();
            t.scale_ = XMFLOAT3(0.3f, 0.3f, 0.3f);
            t.position_ = node->GetPosition();
            Model::SetTransform(nodeHandle, t);
            Model::Draw(nodeHandle);

            //Idの表示
            if (Direct3D::GetCurrentShader() == Direct3D::SHADER_3D) {
                XMFLOAT3 camVPos = Camera::CalcScreenPosition(t.position_);
                camVPos.x = ((camVPos.x + 1.0f) * 0.5f);
                camVPos.y = 1.0f - ((camVPos.y + 1.0f) * 0.5f);
                camVPos.x *= Direct3D::screenWidth_;
                camVPos.y *= Direct3D::screenHeight_;
                pText->Draw((int)camVPos.x, (int)camVPos.y, node->GetId());
            }

            for (Edge edge : node->GetEdges()) {
                //エラー処理
                if (edge.connectId <= -1 || edge.connectId > nodes.size()) continue;

                int count = 5;
                XMFLOAT3 vec = Float3Sub(nodes[edge.connectId]->GetPosition(), node->GetPosition());
                vec = Float3Multiply(vec, 1.0f / ((float)count + 1.0f));
                XMFLOAT3 pos = node->GetPosition();

                float size = 0.05f;
                for (int i = 0; i < count; i++) {
                    pos = Float3Add(pos, vec);
                    float sca = (float)i;
                    t.scale_ = XMFLOAT3(size * sca, size * sca, size * sca);
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

    // ヒューリスティック関数：ユークリッド距離
    float Heuristic(const XMFLOAT3& a, const XMFLOAT3& b) {
        XMFLOAT3 diff;
        XMStoreFloat3(&diff, XMVectorSubtract(XMLoadFloat3(&a), XMLoadFloat3(&b)));
        return std::sqrt(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);
    }

    // A*アルゴリズムの実装
    std::vector<XMFLOAT3> AStar(const std::vector<Node*>& nodes, int start_id, int goal_id) {
        std::unordered_map<int, int> came_from; // 経路再構築のためのマップ
        std::unordered_map<int, float> g_score; // スタートから各ノードまでのコスト
        std::unordered_map<int, float> f_score; // スタートからゴールまでの推定コスト
        std::priority_queue<std::pair<int, float>, std::vector<std::pair<int, float>>, std::greater<>> open_set;
        std::vector<XMFLOAT3> path;

        for (const auto& node : nodes) {
            g_score[node->GetId()] = std::numeric_limits<float>::infinity();
            f_score[node->GetId()] = std::numeric_limits<float>::infinity();
        }

        g_score[start_id] = 0.0f;
        f_score[start_id] = Heuristic(nodes[start_id]->GetPosition(), nodes[goal_id]->GetPosition());
        open_set.emplace(start_id, f_score[start_id]);

        while (!open_set.empty()) {
            int current_id = open_set.top().first;
            open_set.pop();

            if (current_id == goal_id) {
                // 経路の再構築
                while (came_from.find(current_id) != came_from.end()) {
                    path.push_back(nodes[current_id]->GetPosition());
                    current_id = came_from[current_id];
                }
                path.push_back(nodes[start_id]->GetPosition());
                std::reverse(path.begin(), path.end());
                return path;
            }

            for (const auto& edge : nodes[current_id]->GetEdges()) {
                int neighbor_id = edge.connectId;
                float tentative_g_score = g_score[current_id] + edge.cost;

                if (tentative_g_score < g_score[neighbor_id]) {
                    came_from[neighbor_id] = current_id;
                    g_score[neighbor_id] = tentative_g_score;
                    f_score[neighbor_id] = tentative_g_score + Heuristic(nodes[neighbor_id]->GetPosition(), nodes[goal_id]->GetPosition());
                    open_set.emplace(neighbor_id, f_score[neighbor_id]);
                }
            }
        }

        // ゴールへの経路が見つからなかった場合、空のベクトルを返す
        return std::vector<XMFLOAT3>();
    }
}