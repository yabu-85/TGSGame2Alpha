#pragma once
#include "../Engine/GameObject.h"

class Player;
class AimCursor;

//銃を管理するクラス
class GunBase : public GameObject
{
    bool rayHit_;   //Collision判定で保存用
    
    int hModel_;        //モデル番号
    int coolTime_;      //撃つクールダウン
    int topBoneIndex_;  //トップボーンインデックス
    int topPartIndex_;  //トップパーツインデックス
    int rootBoneIndex_; //ルートボーンインデックス
    int rootPartIndex_; //ルートパーツインデックス
    int playerId_;      //プレイヤーID

    Player* pPlayer_;
    AimCursor* pAimCursor_;
public:
    GunBase(GameObject* parent, const std::string& name);
    ~GunBase();
    virtual void OnCollision(GameObject* pTarget) override;

    // 銃弾を生成する
    template<class T>
    void ShootBullet();

    AimCursor* GetAimCursor() { return pAimCursor_; }
};
