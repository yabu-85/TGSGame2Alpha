#pragma once

//��ʂɕ\�������AimCursor�̃N���X
//Draw��PlayScene����Ă΂��
class AimCursor
{
    int hPict_;
    int hHitPict_;
    float accuracyParce_;
    float hitDurace_;   //�߂鎞��
    float hitParce_;    //0.0f �` 1.0f

public:
    AimCursor();
    ~AimCursor();
    void Update();
    void Draw();
    void Hit();
    void SetAccuracyParce(float f) { accuracyParce_ = f; }

};