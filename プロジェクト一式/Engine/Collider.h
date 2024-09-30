#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

//�N���X�̑O���錾
class GameObject;
class BoxCollider;
class SphereCollider;
class CapsuleCollider;
class SegmentCollider;
class Triangle;

#include <vector>
enum class OBJECT_TYPE;

//�����蔻��̃^�C�v
enum ColliderType
{
	COLLIDER_BOX,		//���^
	COLLIDER_CIRCLE,	//����
	COLLIDER_CAPSULE,	//�J�v�Z��
	COLLIDER_SEGMENT,	//����
};

//-----------------------------------------------------------
//�����蔻����Ǘ�����N���X
//-----------------------------------------------------------
class Collider
{
	//���ꂼ��̃N���X��private�����o�ɃA�N�Z�X�ł���悤�ɂ���
	friend class BoxCollider;
	friend class SphereCollider;
	friend class CapsuleCollider;
	friend class SegmentCollider;

public:
	GameObject*		pGameObject_;	//���̔���������Q�[���I�u�W�F�N�g
	ColliderType	type_;			//���
	XMFLOAT3		center_;		//���S�ʒu�i�Q�[���I�u�W�F�N�g�̌��_���猩���ʒu�j
	XMFLOAT3		size_;			//����T�C�Y�i���A�����A���s���j
	int				hDebugModel_;	//�f�o�b�O�\���p�̃��f����ID
	std::vector<OBJECT_TYPE> typeList_;	//���肷�郊�X�g

public:
	//�R���X�g���N�^
	Collider();

	//�f�X�g���N�^
	virtual ~Collider();

	//�e�X�g�\���p�̘g��`��
	//�����Fposition	�I�u�W�F�N�g�̈ʒu
	virtual void Draw(XMFLOAT3 position);

	//�ڐG����i�p�����SphereCollider��BoxCollider�ŃI�[�o�[���C�h�j
	//�����Ftarget	����̓����蔻��
	//�ߒl�F�ڐG���Ă��true
	virtual bool IsHit(Collider* target) = 0;

	//�A�N�Z�T
	ColliderType GetColliderType() { return type_; }
	void SetGameObject(GameObject* gameObject) { pGameObject_ = gameObject; }
	void SetCenter(XMFLOAT3 center) { center_ = center; }
	void SetSize(XMFLOAT3 size) { size_ = size; }

	//�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[

	//���^���m�̏Փ˔���
	//�����FboxA	�P�ڂ̔��^����
	//�����FboxB	�Q�ڂ̔��^����
	//�ߒl�F�ڐG���Ă����true
	bool IsHitBoxVsBox(BoxCollider* boxA, BoxCollider* boxB);

	//���^�Ƌ��̂̏Փ˔���
	//�����Fbox	���^����
	//�����Fsphere	�Q�ڂ̔��^����
	//�ߒl�F�ڐG���Ă����true
	bool IsHitBoxVsCircle(BoxCollider* box, SphereCollider* sphere);

	//���^�Ƌ��̂̏Փ˔���
	//�����Fbox	���^����
	//�����Fsphere	�Q�ڂ̔��^����
	//�ߒl�F�ڐG���Ă����true
	bool IsHitBoxVsCapsule(BoxCollider* box, CapsuleCollider* capsule);

	//���^�ƒ����̏Փ˔���
	//�����Fbox	���^����
	//�����Fseg ��������
	//�ߒl�F�ڐG���Ă����true
	bool IsHitBoxVsSegment(BoxCollider* box, SegmentCollider* seg);

	//�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[
	
	//���̓��m�̏Փ˔���
	//�����FcircleA	�P�ڂ̋��̔���
	//�����FcircleB	�Q�ڂ̋��̔���
	//�ߒl�F�ڐG���Ă����true
	bool IsHitCircleVsCircle(SphereCollider* circleA, SphereCollider* circleB);

	//���̂Ɛ����Փ˔���
	//�����Fcircle	�P�ڂ̋��̔���
	//�����Fcapsule	�Q�ڂ̃J�v�Z������
	//�ߒl�F�ڐG���Ă����true
	bool IsHitCircleVsCapsule(SphereCollider* circle, CapsuleCollider* capsule);

	//���̂Ɛ����Փ˔���
	//�����Fcircle	�P�ڂ̋��̔���
	//�����Fseg	    �Q�ڂ̒�������
	//�ߒl�F�ڐG���Ă����true
	bool IsHitCircleVsSegment(SphereCollider* circle, SegmentCollider* seg);

	//�|���S����Triangle
	//�����o���̃x�N�g�������߂�
	bool IsHitCircleVsTriangle(SphereCollider* circle, Triangle* triangle, XMVECTOR& outDistanceVector);

	//�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[
    
	//�J�v�Z���ƃJ�v�Z���Փ˔���
	//�����Fcapsule1	�P�ڂ̃J�v�Z������
	//�����Fcapsule2	�Q�ڂ̃J�v�Z������
	//�ߒl�F�ڐG���Ă����true
	bool IsHitCapsuleVsCapsule(CapsuleCollider* capsule1, CapsuleCollider* capsule2);

	//�J�v�Z���ƃJ�v�Z���Փ˔���
	//�����Fcapsule1	�P�ڂ̃J�v�Z������
	//�����Fseg			�Q�ڂ̒�������
	//�ߒl�F�ڐG���Ă����true
	bool IsHitCapsuleVsSegment(CapsuleCollider* capsule, SegmentCollider* seg);

};

