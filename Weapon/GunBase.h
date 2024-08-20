#pragma once
#include "BulletBase.h"
#include "../Engine/GameObject.h"

class Player;
class AimCursor;

//銃を管理するクラス
class GunBase : public GameObject
{
protected:
    bool rayHit_;           //Collision判定で保存用
    bool isFirstPerson_;    //一人称視点かどうか(false＝三人称
    bool isPeeking_;        //完全にのぞき込みしている状態か

    int hModel_;            //モデル番号
    int hPlayerModel_;      //プレイヤーのモデル番号
    int hPlayerFPSModel_;   //FPS用モデル番号

    int topBoneIndex_;      //トップボーンインデックス
    int topPartIndex_;      //トップパーツインデックス
    int rootBoneIndex_;     //ルートボーンインデックス
    int rootPartIndex_;     //ルートパーツインデックス
    int handBoneIndex_;     //プレイヤーの手のボーン
    int handPartIndex_;     //プレイヤーの手のパーツ

    int playerId_;                  //プレイヤーID
    int coolTime_;                  //撃つクールダウン
    int peekTime_;                  //覗き込み時間計算用
    
    int reloadTime_;                //リロード時間最大値
    int currentReloadTime_;          //リロード時間保存
    int magazineCount_;             //マガジン最大装弾数
    int currentMagazineCount_;      //マガジン最大装弾数

    Player* pPlayer_;
    AimCursor* pAimCursor_;

    void LoadGunJson(std::string fileName);         //銃の情報読み込み
    void SetGunHandPosition();                      //手の位置に銃のポジションを合わせる
    void Reload();                                  //リロード中の処理
    virtual void ShotBullet(BulletBase* pBullet);   //銃弾発射の処理
    virtual void PressedShot() {};                  //発射のボタン押した
    virtual bool PressedReload() { return false; }; //リロードのボタン押した

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

    //銃弾を生成する
    template<class T>
    inline void ShotBullet()
    {
        BulletBase* pNewBullet = Instantiate<T>(GetParent()->GetParent());
        ShotBullet(pNewBullet);
    }

};
