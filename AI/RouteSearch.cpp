#include "RouteSearch.h"
#include "Node.h"
#include "../Stage/StageEditor.h"
#include "../Engine/Global.h"
#include "../Engine/Model.h"
#include "../Engine/Text.h"
#include "../Engine/Camera.h"

#include <unordered_map>
#include <cmath>
#include <limits>
#include <algorithm>
#include <queue>

namespace RouteSearch {
    bool nodeDraw = false;

    Text* pText = nullptr;
    int nodeHandle = -1;
    int edgeHandle = -1;
    std::vector<Node*> nodes;

    void Initialize()
    {
        pText = new Text();
        pText->Initialize();

        nodes = StageEditor::LoadFileNode("TestStageNode.json");
        nodeHandle = Model::Load("DebugCollision/BoxCollider.fbx");
        edgeHandle = Model::Load("DebugCollision/BoxCollider.fbx");
        assert(nodeHandle >= 0);
        assert(edgeHandle >= 0);
    }

    void Release() {

    }

    std::vector<Node*>& GetNodeList()
    {
        return nodes;
    }

    void NodeModelDraw()
    {
        if (!nodeDraw) return;

        static const float DrawDist = 50.0f;
        for (int i = 0; i < nodes.size(); i++) {
            Node* node = nodes[i];
            XMFLOAT3 vec = Float3Sub(Camera::GetPosition(), node->GetPosition());
            float dist = CalculationDistance(vec);
            //�ő�`�拗��
            if (dist > DrawDist) continue;

            Transform t = Transform();
            t.scale_ = XMFLOAT3(0.4f, 0.4f, 0.4f);
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

                int count = 3;
                XMFLOAT3 vec = Float3Sub(nodes[edge.connectId]->GetPosition(), node->GetPosition());
                vec = Float3Multiply(vec, 1.0f / ((float)count + 1.0f));
                XMFLOAT3 pos = node->GetPosition();

                float size = 0.08f;
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
    std::vector<RouteData> AStar(int goal_id, int start_id)
    {
        //�G���[
        if (nodes.empty()) return std::vector<RouteData>();

        std::unordered_map<int, std::pair<int, EdgeType>> came_from_with_edge; // �o�H�č\�z�̂��߂̃}�b�v (�m�[�hID, (�e�m�[�hID, �g�p���ꂽ�G�b�W�̃^�C�v))
        std::unordered_map<int, float> g_score; // �X�^�[�g����e�m�[�h�܂ł̃R�X�g
        std::unordered_map<int, float> f_score; // �X�^�[�g����S�[���܂ł̐���R�X�g
        std::priority_queue<std::pair<int, float>, std::vector<std::pair<int, float>>, std::greater<>> open_set;

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
                //�o�H�̍č\�z
                std::vector<RouteData> path;
                while (came_from_with_edge.find(current_id) != came_from_with_edge.end()) {
                    RouteData data;
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

    std::vector<RouteData> AStar(int goal_id, const XMFLOAT3& s_pos) {
        int start_id = GetNodeToPosition(s_pos);
        std::vector<RouteData> path = AStar(goal_id, start_id);

        //�X�^�[�g�n�_�̒ǉ�
        RouteData data;
        data.pos = s_pos;
        data.type = EdgeType::NORMAL;
        path.push_back(data);

        PathSmoothing(path);
        return path;
    }

    std::vector<RouteData> AStar(const XMFLOAT3& t_pos, const XMFLOAT3& s_pos)
    {
        int start_id = GetNodeToPosition(s_pos);
        int goal_id = GetNodeToPosition(t_pos);
        std::vector<RouteData> path = AStar(goal_id, start_id);

        //�X�^�[�g�n�_�̒ǉ�
        RouteData data;
        data.pos = s_pos;
        data.type = EdgeType::NORMAL;
        path.push_back(data);
       
        PathSmoothing(path);
        return path;
    }

    void PathSmoothing(std::vector<RouteData>& path) {
        const std::vector<RouteData> prePath = path;
        const float alpha = 0.8f;			//�傫���قǁA����Path�Ɏ��Ă���Path���ł���B�@�@�@�@ �傫���قǏ���������
        const float beta = 0.2f;			//�傫���قǁA�אڂ���_�Ԃł̊��炩�������シ��B�@   �傫���قǏ������x��
        const float tolerance = 0.8f;		//�ω��ʂ����̒l�ȉ��̎����������I���B�@�@�@�@�@�@�@�@ �傫���قǏ���������
        float change = tolerance;			//�p�X�̈ʒu�̕ω���

        while (change >= tolerance) {
            change = 0.0f;

            for (int i = 1; i < path.size() - 1; ++i) {
                XMVECTOR currentPathVector = XMLoadFloat3(&path[i].pos);
                XMVECTOR previousPathVector = XMLoadFloat3(&path[i - 1].pos);
                XMVECTOR nextPathVector = XMLoadFloat3(&path[i + 1].pos);

                XMVECTOR smoothedPathVector = currentPathVector - alpha * (currentPathVector - XMLoadFloat3(&prePath[i].pos));
                smoothedPathVector = smoothedPathVector - beta * (2.0f * currentPathVector - previousPathVector - nextPathVector);
                XMStoreFloat3(&path[i].pos, smoothedPathVector);

                change += XMVectorGetX(XMVector3Length(smoothedPathVector - currentPathVector));
            }
        }
    }
}