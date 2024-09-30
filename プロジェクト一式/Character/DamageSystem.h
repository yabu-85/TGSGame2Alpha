#pragma once
#include <vector>
#include <string>
#include <DirectXMath.h>

using namespace DirectX;
class GameObject;

//ダメージの情報
struct DamageInfo {
    GameObject* owner;   //ダメージを与えるキャラ
    std::string name;   //攻撃の名前
    int damage;         //ダメージ

    DamageInfo() : owner(nullptr), name(""), damage(0) {};
    DamageInfo(int _damage) : owner(nullptr), name(""), damage(_damage) {};
    DamageInfo(GameObject* _owner, std::string _name, int _damage) : owner(_owner), name(_name), damage(_damage) {};
};

enum class KNOCK_TYPE {
    SMALL = 0,
    MEDIUM,
    LARGE,
    MAX,
};

//ノックバックの情報
struct KnockBackInfo {
    KNOCK_TYPE type;    //ノックバックの種類
    int time;           //時間
    float power;        //パワー
    XMFLOAT3 pos;       //ノックバック発生座標

    KnockBackInfo() : type(KNOCK_TYPE::MEDIUM), time(0), power(0.0f), pos({ 0.f, 0.f, 0.f }) {};
    KnockBackInfo(KNOCK_TYPE _type, int _time, float _power, XMFLOAT3 _pos) : type(_type), time(_time), power(_power), pos(_pos) {};
};

class DamageSystem {
	int hp_;            //HP
	int maxHp_;         //最大HP

    int knockBackTime_;             //ノックバックの時間
    int knockBackTimeMax_;          //ノックバックの時間保存用
    XMFLOAT3 knockBackDirection_;   //ノックバックさせる方向
    KNOCK_TYPE knockType_;          //ノックバックのタイプ
    GameObject* pParent_;           //親

	std::vector<DamageInfo> damageInfoList_;    //ダメージを与えられた情報のリスト

public:
    //DamageInfoを考慮しないApplyDamage関数
    void ApplyDamageDirectly(const DamageInfo& daamgeinfo);
    void ApplyDamageDirectly(const DamageInfo& daamgeinfo, const KnockBackInfo& knockinfo);

    /// <summary>
    /// DamageInfoListを考慮するApplyDamage関数
    /// 戻り値：Listになく、攻撃が成功したならtrueが返ってくる
    /// </summary>
    bool ApplyDamageWithList(const DamageInfo& daamgeinfo);
    bool ApplyDamageWithList(const DamageInfo& daamgeinfo, const KnockBackInfo& knockinfo);

    //ダメージ情報
    void AddDamageInfo(const DamageInfo& damageInfo);
    void RemoveDamageInfo(const DamageInfo& damageInfo);

    /// <summary>
    /// ダメージを与えた時に呼ばれる関数
    /// 自動的に呼ばれるわけではないから、手動で関数呼ぶ
    /// </summary>
    /// <param name="damageInfo"></param>
    virtual void OnDamageDealt(const DamageInfo& damageInfo) {};

    /// <summary>
    /// ダメージを与えられたときに呼ばれる関数
    /// ApplyDamageから自動的に呼ばれる
    /// </summary>
    /// <param name="damageInfo"></param>
    virtual void OnDamageReceived(const DamageInfo& damageInfo) {};

    //アクセサ
    void SetHP(int i) { hp_ = i; }
    void SetMaxHP(int i) { maxHp_ = i; }
    int GetHP() { return hp_; }
    int GetMaxHP() { return maxHp_; }

    bool IsHealthZero() { return hp_ <= 0; }

};

/// <summary>
/// 複数回攻撃が当たらないようにするクラス、攻撃をするオブジェクトに持たせて使う
/// attackListに乗っているオブジェクトには攻撃が当たらない
/// </summary>
class DamageController {
    DamageInfo currentDamage;           //今設定されているダメージの情報
    KnockBackInfo currentKnockBack;     //今設定されているノックバックの情報
    std::vector<GameObject*> attackList;//攻撃を与えたリスト

public:
    void AddAttackList(GameObject* obj);        //キャラをリストに登録
    void ResetAttackList();                     //リストリセット

    //ダメージ・ノックバックアクセサ
    void SetCurrentDamage(const DamageInfo& info) { currentDamage = info; }
    DamageInfo& GetCurrentDamage() { return currentDamage; }
    void SetCurrentKnockBackInfo(const KnockBackInfo& info) { currentKnockBack = info; }
    KnockBackInfo& GetCurrentKnockBackInfo() { return currentKnockBack; }
};
