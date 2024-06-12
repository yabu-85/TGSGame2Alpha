#include "DamageSystem.h"
#include "../Engine/Global.h"
#include "../Engine/GameObject.h"

void DamageSystem::ApplyDamageDirectly(const DamageInfo& damageinfo)
{
	hp_ -= damageinfo.damage;

}

void DamageSystem::ApplyDamageDirectly(const DamageInfo& damageinfo, const KnockBackInfo& knockinfo)
{
	//時間かパワー情報あるならノックバックさせる
	if (knockinfo.time != 0 || knockinfo.power != 0.0f) {
		knockBackTime_ = knockinfo.time;
		knockBackTimeMax_ = knockinfo.time;
		knockBackDirection_ = Float3Multiply(Float3Normalize(Float3Sub(pParent_->GetPosition(), knockinfo.pos)), knockinfo.power);
	}

	ApplyDamageDirectly(damageinfo);
}

bool DamageSystem::ApplyDamageWithList(const DamageInfo& damageinfo)
{
	//List似ないなら追加してDamageを与える・すでにあるなら終了
	for (auto it = damageInfoList_.begin(); it != damageInfoList_.end(); ++it) {
		if (it->owner == damageinfo.owner && it->name == damageinfo.name) return false;
	}
	AddDamageInfo(damageinfo);
	ApplyDamageDirectly(damageinfo);

	return true;
}

bool DamageSystem::ApplyDamageWithList(const DamageInfo& damageinfo, const KnockBackInfo& knockinfo)
{
	//List似ないなら追加してDamageを与える・すでにあるなら終了
	for (auto it = damageInfoList_.begin(); it != damageInfoList_.end(); ++it) {
		if (it->owner == damageinfo.owner && it->name == damageinfo.name) return false;
	}
	AddDamageInfo(damageinfo);
	ApplyDamageDirectly(damageinfo, knockinfo);

	return true;
}

void DamageSystem::AddDamageInfo(const DamageInfo& damageInfo)
{
	damageInfoList_.push_back(damageInfo);
}

void DamageSystem::RemoveDamageInfo(const DamageInfo& damageInfo)
{
	for (auto it = damageInfoList_.begin(); it != damageInfoList_.end(); ++it) {
		if (it->owner == damageInfo.owner && it->name == damageInfo.name) {
			damageInfoList_.erase(it);
			break;
		}
	}
}

//--------------------------------------------------------------------------

void DamageController::AddAttackList(GameObject* obj)
{
	attackList.push_back(obj);
}

void DamageController::ResetAttackList()
{
	for (auto i : attackList) {
	//	i->RemoveDamageInfo(currentDamage);
	}
	attackList.clear();
}
