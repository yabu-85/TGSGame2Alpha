#pragma once
#include "../Engine/GameObject.h"
#include "../Engine/EffekseeLib/EffekseerVFX.h"
#include <vector>

class Player;
class AimCursor;
enum class BulletType;

//銃を管理するクラス
class Gun : public GameObject
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
    std::shared_ptr<EFFEKSEERLIB::EFKTransform> mt;

public:
    Gun(GameObject* parent);
    ~Gun();
    void Initialize() override;
    void Update() override;
    void Draw() override;
    void Release() override;
    void OnCollision(GameObject* pTarget) override;

    // 銃弾を生成する
    template<class T>
    void ShootBullet(BulletType type);

    AimCursor* GetAimCursor() { return pAimCursor_; }
};
