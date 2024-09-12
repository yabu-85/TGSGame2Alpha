#pragma once
#include "BulletBase.h"
#include "../Engine/GameObject.h"

class Player;

//銃を管理するクラス
class GunBase : public GameObject
{
protected:
    bool rayHit_;           //Collision判定で保存用
    bool isPeeking_;        //完全にのぞき込みしている状態か

    int hModel_;            //モデル番号
    int hFpsPlayerModel_;   //プレイヤーのモデル番号

    int topBoneIndex_;      //トップボーンインデックス
    int topPartIndex_;      //トップパーツインデックス
    int rootBoneIndex_;     //ルートボーンインデックス
    int rootPartIndex_;     //ルートパーツインデックス
    int handBoneIndex_;     //プレイヤーの手のボーン
    int handPartIndex_;     //プレイヤーの手のパーツ

    int playerId_;                  //プレイヤーID
    int animTime_;                  //アニメーション時間計算
    int coolTime_;                  //撃つクールダウン
    int magazineCount_;             //マガジン最大装弾数
    int currentMagazineCount_;      //マガジン最大装弾数

    int reloadTime_;                //リロード時間最大値
    int currentReloadTime_;         //リロード時間保存

    int peekTime_;                  //覗き込み時間最大値
    int currentPeekTime_;           //覗き込み時間計算用
    float peekZoom_;                //ズームする度合い

    float hipFireAccuracy_;         //腰撃ち時の集弾率
    float aimingAccuracy_;          //覗き込み時の集弾率
    float accuracyDecrease_;        //集弾率を減らす値
    float accuracyRecovery_;        //集弾率を回復させる値
    float currentAccuracy_;         //計算用現在の集弾率
    float maxAccuracy_;             //最大集弾率

    Player* pPlayer_;

    void LoadGunJson(std::string fileName);         //銃の情報読み込み
    void SetGunHandPosition();                      //手の位置に銃のポジションを合わせる
    
    void EnterReload();                             //リロードし始めた
    bool Reload();                                  //リロード中の処理（リロードできたらTrue）
    void FinishedReload();                          //リロード終わった時の処理
    
    void Peeking();                                 //覗き込み処理（アニメーションはしない）
    void ShotVFX();                                 //ShotVFX再生

    virtual void ShotBullet(BulletBase* pBullet);       //銃弾発射の処理
    virtual void ShotFPSBullet(BulletBase* pBullet);    //銃弾発射の処理

    virtual void PressedShot() {};                      //発射のボタン押した
    virtual bool PressedReload() { return false; };     //リロードのボタン押した
    virtual void ResetReload() {};                      //リロードのキャンセル

public:
    GunBase(GameObject* parent, const std::string& name);
    virtual ~GunBase() override;
    virtual void Initialize() override {};
    virtual void Update() override {};
    virtual void Draw() override {};
    virtual void Release() override {};
    virtual void OnCollision(GameObject* pTarget) override;

    // 腰撃 / 覗き撃ち + currentAcct
    float GetAccuracy();

    bool IsPeeking() { return isPeeking_; }
    int GetCurrentMagazineCount() { return currentMagazineCount_; }
    int GetMaxMagazineCount() { return magazineCount_; }
    int GetReloadTime() { return reloadTime_; }

    //銃弾を生成する
    template<class T>
    inline BulletBase* ShotBullet(std::string jsonName)
    {
        BulletBase* pNewBullet = Instantiate<T>(GetParent()->GetParent());
        pNewBullet->LoadBulletParameter(jsonName);
        ShotBullet(pNewBullet);
        return pNewBullet;
    }

    //銃弾を生成する
    template<class T>
    inline BulletBase* ShotFpsBullet(std::string jsonName)
    {
        BulletBase* pNewBullet = Instantiate<T>(GetParent()->GetParent());
        pNewBullet->LoadBulletParameter(jsonName);
        ShotFPSBullet(pNewBullet);
        return pNewBullet;
    }

};
