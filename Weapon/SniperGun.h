#pragma once
#include "GunBase.h"

class Player;
class AimCursor;

//銃を管理するクラス
class SniperGun : public GunBase
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
    SniperGun(GameObject* parent);
    ~SniperGun();
    void Initialize() override;
    void Update() override;
    void Draw() override;
    void Release() override;

};
