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
            if (dist > 40.0f) continue;

            Transform t = Transform();
            t.scale_ = XMFLOAT3(0.3f, 0.3f, 0.3f);
            t.position_ = node->GetPosition();
            Model::SetTransform(nodeHandle, t);
            Model::Draw(nodeHandle);

            //Id�̕\��
            if (Direct3D::GetCurrentShader() == Direct3D::SHADER_3D) {
                XMFLOAT3 camVPos = Camera::CalcScreenPosition(t.position_);
                camVPos.x = ((camVPos.x + 1.0f) * 0.5f);
                camVPos.y = 1.0f - ((camVPos.y + 1.0f) * 0.5f);
                camVPos.x *= Direct3D::screenWidth_;
                camVPos.y *= Direct3D::screenHeight_;
                pText->Draw((int)camVPos.x, (int)camVPos.y, node->GetId());
            }

            for (Edge edge : node->GetEdges()) {
                //�G���[����
                if (edge.connectId <= -1 || (edge.connectId > nodes.size() - 1)) continue;

                int count = 5;
                XMFLOAT3 vec = Float3Sub(nodes[edge.connectId]->GetPosition(), node->GetPosition());
                vec = Float3Multiply(vec, 1.0f / ((float)count + 1.0f));
                XMFLOAT3 pos = node->GetPosition();

                float size = 0.05f;
                for (int i = 0; i < count; i++) {
                    pos = Float3Add(pos, vec);
                    float sca = (float)count - (float)i;
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

    int GetNodeToPosition(const XMFLOAT3& pos)
    {
        float minDist = 999999;
        int minId = -1;
        for (int i = 0; i < nodes.size(); i++) {
            Node* node = nodes[i];
            float dist = CalculationDistance(pos, node->GetPosition());
            if (dist <= minDist) {
                minDist = dist;
                minId = nodes[i]->GetId();
            }
        }
        return minId;
    }

    // �q���[���X�e�B�b�N�֐��F���[�N���b�h����
    float Heuristic(const XMFLOAT3& a, const XMFLOAT3& b) {
        XMFLOAT3 diff;
        XMStoreFloat3(&diff, XMVectorSubtract(XMLoadFloat3(&a), XMLoadFloat3(&b)));
        return std::sqrt(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);
    }

    // A*�A���S���Y���̎���
    std::vector<RouteData> AStar(const std::vector<Node*>& nodes, int goal_id, const XMFLOAT3& pos) {
        std::unordered_map<int, std::pair<int, EdgeType>> came_from_with_edge; // �o�H�č\�z�̂��߂̃}�b�v (�m�[�hID, (�e�m�[�hID, �g�p���ꂽ�G�b�W�̃^�C�v))
        std::unordered_map<int, float> g_score; // �X�^�[�g����e�m�[�h�܂ł̃R�X�g
        std::unordered_map<int, float> f_score; // �X�^�[�g����S�[���܂ł̐���R�X�g
        std::priority_queue<std::pair<int, float>, std::vector<std::pair<int, float>>, std::greater<>> open_set;

        for (const auto& node : nodes) {
            g_score[node->GetId()] = std::numeric_limits<float>::infinity();
            f_score[node->GetId()] = std::numeric_limits<float>::infinity();
        }

        int start_id = GetNodeToPosition(pos);
        g_score[start_id] = 0.0f;
        f_score[start_id] = Heuristic(nodes[start_id]->GetPosition(), nodes[goal_id]->GetPosition());
        open_set.emplace(start_id, f_score[start_id]);

        while (!open_set.empty()) {
            int current_id = open_set.top().first;
            open_set.pop();

            if (current_id == goal_id) {
                //�o�H�̍č\�z
                std::vector<RouteData> path;
                RouteData data;
                data.pos = pos;
                data.type = EdgeType::NORMAL;
                path.push_back(data);

                while (came_from_with_edge.find(current_id) != came_from_with_edge.end()) {
                    data.pos = nodes[current_id]->GetPosition();
                    data.type = came_from_with_edge[current_id].second;
                    path.push_back(data);
                    current_id = came_from_with_edge[current_id].first;
                }
                return path;
            }

            for (const auto& edge : nodes[current_id]->GetEdges()) {
                int neighbor_id = edge.connectId;
                float tentative_g_score = g_score[current_id] + edge.cost;

                if (tentative_g_score < g_score[neighbor_id]) {
                    came_from_with_edge[neighbor_id] = std::make_pair(current_id, edge.type);
                    g_score[neighbor_id] = tentative_g_score;
                    f_score[neighbor_id] = tentative_g_score + Heuristic(nodes[neighbor_id]->GetPosition(), nodes[goal_id]->GetPosition());
                    open_set.emplace(neighbor_id, f_score[neighbor_id]);
                }
            }
        }

        //�S�[���܂ł��ǂ蒅���Ȃ�����
        return std::vector<RouteData>();
    }
}