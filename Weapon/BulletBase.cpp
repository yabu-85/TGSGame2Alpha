#include "BulletBase.h"

BulletBase::BulletBase(GameObject* parent, std::string name)
	: GameObject(parent, name), playerId_(0)
{
}
