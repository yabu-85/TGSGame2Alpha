#pragma once
#include "../Engine/GameObject.h"

class Character;

// 銃弾の基底クラス
class BulletBase : public GameObject
{
    // バレットの共通パラメータ
    struct BulletParameter
    {
        int damage_;            //ダメージ
        int shotCoolTime_;      //クールタイム
        float speed_;           //スピード
        int killTimer_;         //消失時間
        float collisionScale_;  //当たり判定のサイズ
        int isPenetration_;     //貫通   あり:1 なし:0
    };
protected:
    int playerId_;              //ダメージ描画のための
    BulletParameter parameter_; //パラメータ
    XMFLOAT3 move_;             //移動の速度と方向

public:
    BulletBase(GameObject* parent, std::string name);
    virtual ~BulletBase() = default;

    virtual void Shot(Character* chara, XMFLOAT3 wallHitPos, XMFLOAT3 charaHitPos) {};

    // 銃弾のパラメータを取得
    BulletParameter GetBulletParameter() const { return parameter_; }

    // 銃弾の移動を設定
    void SetMove(const XMFLOAT3& move) { move_ = move; }

    void SetPlayerId(int id) { playerId_ = id; }

};