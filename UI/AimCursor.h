#pragma once

//��ʂɕ\�������AimCursor�̃N���X
//Draw��PlayScene����Ă΂��
class AimCursor
{
    int hPict_;
    int hHitPict_;

    float shotDurace_;  //�߂鎞��
    float shotParce_;   //0.0f �` 1.0f�̎��Ԋ���
    float hitDurace_;   //�߂鎞��
    float hitParce_;    //0.0f �` 1.0f

public:
    AimCursor();
    ~AimCursor();
    void Update();
    void Draw();
    void Shot();
    void Hit();

    float GetBurePower();
};