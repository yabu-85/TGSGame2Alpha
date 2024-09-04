#include "BulletBase.h"
#include "../Json/JsonReader.h"

BulletBase::BulletBase(GameObject* parent, std::string name)
	: GameObject(parent, name), playerId_(0), move_(XMFLOAT3()), parameter_(BulletParameter())
{
}

void BulletBase::LoadBulletParameter(std::string name)
{
    //JSONファイル読み込み
    JsonReader::Load("Json/Bullet.json");
    auto& Bullet_Normal = JsonReader::GetSection(name);

    //パラメータを取得
    parameter_.damage_ = Bullet_Normal["damage"];
    parameter_.shotCoolTime_ = Bullet_Normal["shotCoolTime"];
    parameter_.speed_ = Bullet_Normal["speed"];
    parameter_.killTimer_ = Bullet_Normal["killTimer"];
    parameter_.collisionScale_ = Bullet_Normal["collisionScale"];

}
