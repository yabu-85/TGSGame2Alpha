#include "VFXManager.h"
#include "../Engine/EffekseeLib/EffekseerVFX.h"

namespace VFXManager {
    void Initialize()
    {
        EFFEKSEERLIB::gEfk->AddEffect("HIT", "Particle/hit.efk");
        EFFEKSEERLIB::gEfk->AddEffect("TAMA", "Particle/blurParticle.efk");
        EFFEKSEERLIB::gEfk->AddEffect("GUNSHOT", "Particle/gunShot.efk");

    }

    void CreateVfx(XMFLOAT3 pos, VFX_TYPE type, XMFLOAT3 other)
    {
        switch (type)
        {
        case Explode:		CreateVfxExplode(pos);			break;
        default: break;
        }
    }

    void CreateVfxExplode(XMFLOAT3 pos)
    {
        Transform trans;
        trans.position_ = pos;

        EFFEKSEERLIB::EFKTransform t;
        DirectX::XMStoreFloat4x4(&(t.matrix), trans.GetWorldMatrix());
        t.isLoop = false;
        t.maxFrame = 80;
        t.speed = 1.0;
        EFFEKSEERLIB::gEfk->Play("HIT", t);
    }

}