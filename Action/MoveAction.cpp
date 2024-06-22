#include "MoveAction.h"
#include "../Player/Player.h"
#include "../Stage/CollisionMap.h"
#include "../Enemy/EnemyManager.h"
#include "../Enemy/EnemyBase.h"
#include "../Engine/Global.h"
#include "../Engine/GameObject.h"
#include "../Engine/Model.h"
#include "../Other/GameManager.h"

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
	XMFLOAT3 fPos = pCharacter_->GetPosition();
	XMVECTOR vPos = XMLoadFloat3(&fPos);
	XMVECTOR vTar = XMLoadFloat3(&targetPos_);
	XMVECTOR vMove = vTar - vPos;
	
	//Target�ʒu����
	float tarDist = XMVectorGetX(XMVector3Length(vMove));
	if (tarDist <= moveRange_) {
		isInRange_ = true;
		return;
	}

	//�ړ��X�s�[�h�}�����Ĉړ�
	if(tarDist > moveSpeed_) vMove = XMVector3Normalize(vMove) * moveSpeed_;
	XMStoreFloat3(&fPos, vPos + vMove);
	pCharacter_->SetPosition(fPos);
	isInRange_ = false;
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

	XMFLOAT3 fPos = pCharacter_->GetPosition();
	XMVECTOR vPos = XMLoadFloat3(&fPos);
	XMVECTOR vTar = XMLoadFloat3(&targetList_.back().pos);
	XMVECTOR vMove = vTar - vPos;

	if (/*jump*/ false) {

	}

	//�^�[�Q�b�g�ʒu����
	float moveDist = XMVectorGetX(XMVector3Length(vMove));
	if (moveDist <= moveRange_) {
		targetList_.pop_back();
		Update();
		return;
	}

	//�ړ��X�s�[�h�}��
	if (moveDist > moveSpeed_) vMove = XMVector3Normalize(vMove) * moveSpeed_;

	//����v�Z
	CalcDodge(vMove);

	XMStoreFloat3(&fPos, vPos + vMove);
	pCharacter_->SetPosition(fPos);

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
	targetList_ = RouteSearch::AStar(target, pCharacter_->GetPosition());
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


*/