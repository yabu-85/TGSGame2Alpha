#pragma once

//��ʂɕ\�������AimCursor�̃N���X
//Draw��PlayScene����Ă΂��
class AimCursor
{
    int hPict_;

    float shotDurace_;  //�߂鎞��
    float shotParce_;   //0.0f �` 1.0f�̎��Ԋ���

public:
    AimCursor();
    ~AimCursor();
    void Update();
    void Draw();

    void Shot();

};