#pragma once
#include "../Engine/GameObject.h"

class Player;
class AimCursor;
class BulletBase;

//銃を管理するクラス
class GunBase : public GameObject
{
protected:
    bool rayHit_;       //Collision判定で保存用
    bool isFirstPerson_;//一人称視点かどうか(false＝三人称

    int hModel_;        //モデル番号
    int coolTime_;      //撃つクールダウン
    int topBoneIndex_;  //トップボーンインデックス
    int topPartIndex_;  //トップパーツインデックス
    int rootBoneIndex_; //ルートボーンインデックス
    int rootPartIndex_; //ルートパーツインデックス
    int playerId_;      //プレイヤーID

    Player* pPlayer_;
    AimCursor* pAimCursor_;

    void ShootBulletCalc(BulletBase* pBullet);

public:
    GunBase(GameObject* parent, const std::string& name);
    virtual ~GunBase() override;
    virtual void Initialize() override = 0;
    virtual void Update() override = 0;
    virtual void Draw() override = 0;
    virtual void Release() override = 0;
    virtual void OnCollision(GameObject* pTarget) override;

    // 銃弾を生成する
    template<class T>
    void ShootBullet();

    bool IsFirstPerson() { return isFirstPerson_; }
    AimCursor* GetAimCursor() { return pAimCursor_; }
};
