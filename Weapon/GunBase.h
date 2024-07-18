#pragma once
#include "BulletBase.h"
#include "../Engine/GameObject.h"

class Player;
class AimCursor;

//銃を管理するクラス
class GunBase : public GameObject
{
protected:
    bool rayHit_;       //Collision判定で保存用
    bool isFirstPerson_;//一人称視点かどうか(false＝三人称
    bool isPeeking_;

    int hModel_;        //モデル番号
    int coolTime_;      //撃つクールダウン
    int topBoneIndex_;  //トップボーンインデックス
    int topPartIndex_;  //トップパーツインデックス
    int rootBoneIndex_; //ルートボーンインデックス
    int rootPartIndex_; //ルートパーツインデックス
    int playerId_;      //プレイヤーID

    int peekTime_;      //覗き込み時間計算用

    Player* pPlayer_;
    AimCursor* pAimCursor_;

    void ShootBullet(BulletBase* pBullet);
public:
    GunBase(GameObject* parent, const std::string& name);
    virtual ~GunBase() override {};
    virtual void Initialize() override {};
    virtual void Update() override {};
    virtual void Draw() override {};
    virtual void Release() override {};
    virtual void OnCollision(GameObject* pTarget) override;

    bool IsFirstPerson() { return isFirstPerson_; }
    bool IsPeeking() { return isPeeking_; }
    AimCursor* GetAimCursor() { return pAimCursor_; }

    // 銃弾を生成する
    template<class T>
    void ShootBullet()
    {
        BulletBase* pNewBullet = Instantiate<T>(GetParent()->GetParent());
        ShootBullet(pNewBullet);
    }
};
