#include "MoveAction.h"
#include "../Player/Player.h"
#include "../Stage/CollisionMap.h"
#include "../Enemy/EnemyManager.h"
#include "../Enemy/EnemyBase.h"
#include "../Engine/Global.h"
#include "../Engine/GameObject.h"
#include "../Engine/Model.h"
#include "../AI/RouteSearch.h"

namespace {
	//Orient�Ɏg�p
	static const float DIR_X[4] = { 0, 1, 0, -1 };
	static const float DIR_Z[4] = { 1, 0, -1, 0 };
	enum DIRECTION {
		FRONT = 0,
		RIGHT,
		BACK,
		LEFT,
	};
}

MoveAction::MoveAction(GameObject* obj, float speed, float range)
	: BaseAction(obj), isInRange_(false), moveSpeed_(speed), moveRange_(range), targetPos_(0, 0, 0)
{
}

void MoveAction::Update()
{
	//�ړ��ʌv�Z
	XMFLOAT3 pos = pCharacter_->GetPosition();
	XMVECTOR vPos = XMLoadFloat3(&pos);
	XMVECTOR vTar = XMLoadFloat3(&targetPos_);
	XMVECTOR vMove = vTar - vPos;
	
	//�ړ��X�s�[�h�}��
	float currentSpeed = XMVectorGetX(XMVector3Length(vTar - vPos));
	if(currentSpeed > moveSpeed_) vMove = XMVector3Normalize(vMove) * moveSpeed_;

	//Target�ʒu����
	float length = XMVectorGetX(XMVector3Length(vTar - vPos));
	if (length <= moveRange_) {
		isInRange_ = true;
	}

	XMStoreFloat3(&pos, vPos + vMove);
	pCharacter_->SetPosition(pos);
	isInRange_ = false;;
}

void MoveAction::CalcDodge(XMVECTOR& move)
{
	const float safeSize = 2.5f;
	XMFLOAT3 pos = pCharacter_->GetPosition();
	XMVECTOR vPos = XMLoadFloat3(&pos);
	XMVECTOR vSafeMove = XMVectorZero();
	
	for (auto& e : EnemyManager::GetAllEnemy()) {
		if (e == pCharacter_) continue;
		XMFLOAT3 f = e->GetPosition();
		XMVECTOR vTarget = XMLoadFloat3(&f);
		XMVECTOR vec = vTarget - vPos;
		float range = XMVectorGetX(XMVector3Length(vec));

		if (range < safeSize) {
			range -= safeSize;
			vSafeMove += XMVector3Normalize(vec) * range;
		}
	}

	move += vSafeMove;
}

//------------------------------Astar----------------------

AstarMoveAction::AstarMoveAction(GameObject* obj, float speed, float range) : MoveAction(obj, speed, range), lastTarget_(0,0,0), handle_(-1)
{
#ifdef _DEBUG
	handle_ = Model::Load("Model/Mas.fbx");
	assert(handle_ >= 0);
#endif

}

void AstarMoveAction::Update()
{
	isInRange_ = false;

	//�ړ��I������
	if (targetList_.empty()) {
		isInRange_ = true;
		return;
	}
	
	//�ړ������v�Z
	XMFLOAT3 pos = pCharacter_->GetPosition();
	XMVECTOR vPos = XMLoadFloat3(&pos);
	XMVECTOR vTar = XMLoadFloat3(&targetList_.back().pos);
	XMVECTOR vMove = vTar - vPos;

	//�ړ��X�s�[�h�}��
	float currentSpeed = XMVectorGetX(XMVector3Length(vMove));
	if (currentSpeed > moveSpeed_) vMove = XMVector3Normalize(vMove) * moveSpeed_;

	//Target�ʒu�����F�J�N�J�N���Ȃ��悤�ɍċN��������
	float length = XMVectorGetX(XMVector3Length(vTar - vPos));

	//CalcDodge�g���ꍇ��moveRange�{vSafeMove�ł�����ق�������
	if (length <= moveRange_) {
		targetList_.pop_back();
		Update();
		return;
	}

	XMStoreFloat3(&pos, vPos + vMove);
	pCharacter_->SetPosition(pos);
}

bool AstarMoveAction::IsOutTarget(float range)
{
	//lastTarget���X�V�EoutTarget�֐��ȊO�ł��g���Ȃ�Update�ɒu�����ق�������
	if (!targetList_.empty()) {
		lastTarget_ = targetList_.front().pos;
	}

	//range�O������true
	if (range < CalculationDistance(targetPos_, lastTarget_)) return true;
	return false;
}

void AstarMoveAction::UpdatePath(XMFLOAT3 target)
{
	targetList_ = RouteSearch::AStar(RouteSearch::GetNodeList(), pCharacter_->GetPosition(), target);
	if(!targetList_.empty()) targetPos_ = targetList_.front().pos;
}

void AstarMoveAction::Draw()
{
#ifdef _DEBUG
	Transform t;
	for (int i = 0; i < targetList_.size(); i++) {
		t.position_ = targetList_.at(i).pos;
		t.position_.y += 0.01f;
		Model::SetTransform(handle_, t);
		Model::Draw(handle_);
	}
#endif
}

//------------------------------Oriented----------------------

OrientedMoveAction::OrientedMoveAction(GameObject* obj, float speed)
	: MoveAction(obj, speed, 0.0f), direction_ { 0, 0, -1, 0 }, move_{XMVectorZero()}, targetDirection_(XMVectorZero())
{
}

void OrientedMoveAction::Update() {
	//�^�[�Q�b�g�ւ̕�����rotateY���v�Z
	float rotationY = atan2f(XMVectorGetX(targetDirection_), XMVectorGetZ(targetDirection_));

	//���̕������Ɉړ�
	XMMATRIX mRotY = XMMatrixRotationY(rotationY);
	XMVECTOR vMove = XMVector3TransformCoord(direction_, mRotY);
	vMove = XMVector3Normalize(vMove);
	move_ = vMove * moveSpeed_;
	
	XMFLOAT3 position = pCharacter_->GetPosition();
	XMVECTOR vPosition = XMLoadFloat3(&position); 
	XMStoreFloat3(&position, vPosition + move_);
	pCharacter_->SetPosition(position);
}

void OrientedMoveAction::SetTarget(XMFLOAT3 target)
{
	targetPos_ = target;
	XMFLOAT3 position = pCharacter_->GetPosition();
	XMVECTOR vPosition = XMLoadFloat3(&position); 
	targetDirection_ = XMLoadFloat3(&targetPos_) - vPosition;
	targetDirection_ = XMVector3Normalize(targetDirection_);
}

void OrientedMoveAction::InverseDirection()
{
	direction_ *= -1.0f;
}


/*
static const float RayMoveDist = 10.0f;
	static const float RayHeight = 0.5f;

	XMVECTOR vPos2 = XMLoadFloat3(&transform_.position_);
	XMFLOAT3 playerPos = pPlayer->GetPosition();
	XMVECTOR vPlaPos2 = XMLoadFloat3(&playerPos);
	XMVECTOR vec2 = vPos2 - vPlaPos2;

	float plaDist = XMVectorGetX(XMVector3Length(vec2));
	if (plaDist <= 1.0f) {
		targetList_.clear();
		return;
	}

	if (plaDist <= RayMoveDist) {

		if (rand() % 10 == 0) {
			RayCastData rayData = RayCastData();
			rayData.start = XMFLOAT3(transform_.position_.x, transform_.position_.y + RayHeight, transform_.position_.z);

			XMVECTOR vec = XMVector3Normalize(vec2) * -1.0f;
			XMStoreFloat3(&rayData.dir, vec);

			XMFLOAT3 target = XMFLOAT3(pPlayer->GetPosition().x, pPlayer->GetPosition().y + RayHeight, pPlayer->GetPosition().z);
			pCMap->RaySelectCellVsSegment(target, &rayData);
			moveReady = rayData.dist >= plaDist;
		}

		if (moveReady) {
			targetList_.clear();

			//�ړ��X�s�[�h�}��
			float moveDist = XMVectorGetX(XMVector3Length(vec2));
			if (moveDist > MoveSpeed) vec2 = XMVector3Normalize(vec2) * MoveSpeed;

			//����v�Z
			CalcDodge(vec2);

			//�ړ�
			vPos2 -= vec2;
			XMStoreFloat3(&transform_.position_, vPos2);
			return;
		}
	}

	//���̎��ԂŁA���X�g�o�H�X�V����
	if ( (targetList_.empty() || rand() % 30 == 0) && rand() % 10 == 0) {
		targetList_ = RouteSearch::AStar(RouteSearch::GetNodeList(), pPlayer->GetPosition(), transform_.position_);
	}

	//�ړ��I������
	if (targetList_.empty()) {
		return;
	}

	//�W�����v�ړ�
	if (targetList_.back().type == EdgeType::JUMP) {
		gravity_ = 0.0f;
		//�ړ������v�Z
		XMVECTOR vPos = XMLoadFloat3(&transform_.position_);
		XMVECTOR vTar = XMLoadFloat3(&targetList_.back().pos);
		XMVECTOR vMove = vTar - vPos;

		//�ړ��X�s�[�h�}��
		float moveDist = XMVectorGetX(XMVector3Length(vMove));
		if (moveDist > MoveSpeed) vMove = XMVector3Normalize(vMove) * MoveSpeed;

		//�^�[�Q�b�g�ʒu����
		if (moveDist <= moveRange_) {
			targetList_.pop_back();
			Move();
			return;
		}

		vPos += vMove;
		XMStoreFloat3(&transform_.position_, vPos);
		return;
	}

	//�ړ������v�Z
	XMVECTOR vPos = XMLoadFloat3(&transform_.position_);
	XMVECTOR vTar = XMLoadFloat3(&targetList_.back().pos);
	XMVECTOR vMove = vTar - vPos;

	//�ړ��X�s�[�h�}��
	float moveDist = XMVectorGetX(XMVector3Length(vMove));
	if (moveDist > MoveSpeed) vMove = XMVector3Normalize(vMove) * MoveSpeed;

	//�^�[�Q�b�g�ʒu����
	if (moveDist <= moveRange_) {
		targetList_.pop_back();
		Move();
		return;
	}

	//����v�Z
	CalcDodge(vMove);

	vPos += vMove;
	XMStoreFloat3(&transform_.position_, vPos);*/