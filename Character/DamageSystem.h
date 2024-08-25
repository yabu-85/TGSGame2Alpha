#pragma once
#include <vector>
#include <string>
#include <DirectXMath.h>

using namespace DirectX;
class GameObject;

//�_���[�W�̏��
struct DamageInfo {
    GameObject* owner;   //�_���[�W��^����L����
    std::string name;   //�U���̖��O
    int damage;         //�_���[�W

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

//�m�b�N�o�b�N�̏��
struct KnockBackInfo {
    KNOCK_TYPE type;    //�m�b�N�o�b�N�̎��
    int time;           //����
    float power;        //�p���[
    XMFLOAT3 pos;       //�m�b�N�o�b�N�������W

    KnockBackInfo() : type(KNOCK_TYPE::MEDIUM), time(0), power(0.0f), pos({ 0.f, 0.f, 0.f }) {};
    KnockBackInfo(KNOCK_TYPE _type, int _time, float _power, XMFLOAT3 _pos) : type(_type), time(_time), power(_power), pos(_pos) {};
};

class DamageSystem {
	int hp_;            //HP
	int maxHp_;         //�ő�HP

    int knockBackTime_;             //�m�b�N�o�b�N�̎���
    int knockBackTimeMax_;          //�m�b�N�o�b�N�̎��ԕۑ��p
    XMFLOAT3 knockBackDirection_;   //�m�b�N�o�b�N���������
    KNOCK_TYPE knockType_;          //�m�b�N�o�b�N�̃^�C�v
    GameObject* pParent_;           //�e

	std::vector<DamageInfo> damageInfoList_;    //�_���[�W��^����ꂽ���̃��X�g

public:
    //DamageInfo���l�����Ȃ�ApplyDamage�֐�
    void ApplyDamageDirectly(const DamageInfo& daamgeinfo);
    void ApplyDamageDirectly(const DamageInfo& daamgeinfo, const KnockBackInfo& knockinfo);

    /// <summary>
    /// DamageInfoList���l������ApplyDamage�֐�
    /// �߂�l�FList�ɂȂ��A�U�������������Ȃ�true���Ԃ��Ă���
    /// </summary>
    bool ApplyDamageWithList(const DamageInfo& daamgeinfo);
    bool ApplyDamageWithList(const DamageInfo& daamgeinfo, const KnockBackInfo& knockinfo);

    //�_���[�W���
    void AddDamageInfo(const DamageInfo& damageInfo);
    void RemoveDamageInfo(const DamageInfo& damageInfo);

    /// <summary>
    /// �_���[�W��^�������ɌĂ΂��֐�
    /// �����I�ɌĂ΂��킯�ł͂Ȃ�����A�蓮�Ŋ֐��Ă�
    /// </summary>
    /// <param name="damageInfo"></param>
    virtual void OnDamageDealt(const DamageInfo& damageInfo) {};

    /// <summary>
    /// �_���[�W��^����ꂽ�Ƃ��ɌĂ΂��֐�
    /// ApplyDamage���玩���I�ɌĂ΂��
    /// </summary>
    /// <param name="damageInfo"></param>
    virtual void OnDamageReceived(const DamageInfo& damageInfo) {};

    //�A�N�Z�T
    void SetHP(int i) { hp_ = i; }
    void SetMaxHP(int i) { maxHp_ = i; }
    int GetHP() { return hp_; }
    int GetMaxHP() { return maxHp_; }

    bool IsHealthZero() { return hp_ <= 0; }

};

/// <summary>
/// ������U����������Ȃ��悤�ɂ���N���X�A�U��������I�u�W�F�N�g�Ɏ������Ďg��
/// attackList�ɏ���Ă���I�u�W�F�N�g�ɂ͍U����������Ȃ�
/// </summary>
class DamageController {
    DamageInfo currentDamage;           //���ݒ肳��Ă���_���[�W�̏��
    KnockBackInfo currentKnockBack;     //���ݒ肳��Ă���m�b�N�o�b�N�̏��
    std::vector<GameObject*> attackList;//�U����^�������X�g

public:
    void AddAttackList(GameObject* obj);        //�L���������X�g�ɓo�^
    void ResetAttackList();                     //���X�g���Z�b�g

    //�_���[�W�E�m�b�N�o�b�N�A�N�Z�T
    void SetCurrentDamage(const DamageInfo& info) { currentDamage = info; }
    DamageInfo& GetCurrentDamage() { return currentDamage; }
    void SetCurrentKnockBackInfo(const KnockBackInfo& info) { currentKnockBack = info; }
    KnockBackInfo& GetCurrentKnockBackInfo() { return currentKnockBack; }
};
