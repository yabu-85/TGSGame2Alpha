#pragma once
#include "../Engine/GameObject.h"
#include <vector>

class Player;
enum class BulletType;

// バレットの情報を保管する構造体
struct BulletInfo
{
    int coolTime_;
};


//銃を管理するクラス
class Gun : public GameObject
{
    bool rayHit_;   //Collision判定で保存用
    
    int hModel_;        //モデル番号
    int topBoneIndex_;  //トップボーンインデックス
    int topPartIndex_;  //トップパーツインデックス
    int rootBoneIndex_; //ルートボーンインデックス
    int rootPartIndex_; //ルートパーツインデックス
    int playerId_;      //プレイヤーID

    BulletInfo bulletInfo_;
    Player* pPlayer_;

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
};
