#pragma once
#include "../Engine/GameObject.h"
#include <vector>

enum class BulletType;

// バレットの情報を保管する構造体
struct BulletInfo
{
    int coolTime_;
};


//銃を管理するクラス
class Gun : public GameObject
{
    int hModel_;                    // モデル番号
    XMFLOAT3 moveDirection_;        // 動く方向

    // バレットのリスト
    std::vector<BulletInfo> bulletInfoList_;

public:
    Gun(GameObject* parent);
    ~Gun();
    void Initialize() override;
    void Update() override;
    void Draw() override;
    void Release() override;

    // 銃弾の速度と射出方向を計算する
    XMFLOAT3 CalculateBulletMovement(XMFLOAT3 top, XMFLOAT3 root, float bulletSpeed);

    // 銃弾を生成する
    template<class T>
    void ShootBullet(BulletType type);

    // 移動方向取得
    XMFLOAT3 GetMoveDirection() const { return moveDirection_; }
};
