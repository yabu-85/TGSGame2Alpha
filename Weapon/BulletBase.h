#pragma once
#include "../Engine/GameObject.h"

class Character;

//銃弾の基底クラス
class BulletBase : public GameObject
{
protected:
    //バレットの共通パラメータ
    struct BulletParameter
    {
        int damage_;            //ダメージ
        int killTimer_;         //消失時間
        int shotCoolTime_;      //クールタイム
        float speed_;           //スピード
        float collisionScale_;  //当たり判定のサイズ
    } parameter_;               //弾丸パラメータ

    int playerId_;              //ダメージ描画のための
    XMFLOAT3 move_;             //移動の速度と方向
    XMFLOAT3 drawPos_;          //表示用の座標
    XMFLOAT3 drawMove_;         //表示用の移動ベクトル

public:
    BulletBase(GameObject* parent, std::string name);
    virtual ~BulletBase() = default;

    virtual void Shot(Character* chara, XMFLOAT3 wallHitPos, XMFLOAT3 charaHitPos) {};

    //銃弾のパラメータを取得
    BulletParameter GetBulletParameter() { return parameter_; }
    void LoadBulletParameter(std::string name);

    void SetPlayerId(int id) { playerId_ = id; }
    void SetMove(XMFLOAT3 move) { move_ = move; }
    void SetDrawMove(XMFLOAT3 move) { drawMove_ = move; }
    void SetDrawPosition(XMFLOAT3 pos) { drawPos_ = pos; }

};