#include "BulletBase.h"

BulletBase::BulletBase(GameObject* parent, std::string name)
	: GameObject(parent, name), playerId_(0), move_(XMFLOAT3()), parameter_(BulletParameter())
{
}
