#include "BulletBase.h"

BulletBase::BulletBase(GameObject* parent, BulletType bulletType, std::string name)
	: GameObject(parent, name), bulletType_(bulletType)
{
}
