#include "DamageUI.h"
#include "../Engine/Transform.h"
#include "../Engine/Direct3D.h"
#include "../Engine/Camera.h"
#include "../Engine/Text.h"
#include <vector>

namespace {

}

struct DamageUIInfo {
	int damage;
	int alpha;
	float time;
	XMFLOAT3 wPos;
};

namespace DamageUI {
	std::vector<DamageUIInfo> damageList_;
	Text* pText_ = nullptr;

	void Initialize()
	{
		pText_ = new Text;
		pText_->Initialize();
	}

	void AddDamage(XMFLOAT3 _pos, int _damage)
	{
		DamageUIInfo ui;
		ui.damage = _damage;
		ui.alpha = 255;
		ui.time = 1.0f;

		//ポジションずらして設定
		ui.wPos = _pos;
		ui.wPos.x += (float)(rand() % 100 * 0.01f - 0.5f);
		ui.wPos.y += (float)(rand() % 100 * 0.01f - 0.5f);

		damageList_.push_back(ui);
	}

	void ResetDamageList()
	{
		damageList_.clear();
	}

	void Update()
	{
		for (DamageUIInfo& ui : damageList_) {
			ui.time -= 0.02f;
			if (ui.time <= 0.2) {
				ui.wPos.y += 0.02f;
				ui.alpha -= 20;

				//リストから削除、末尾から消してくといいと思う
				if (ui.time <= 0.0f) damageList_.erase(ui);
			}
		}
	}

	void Draw()
	{
		Direct3D::SetBlendMode(Direct3D::BLEND_DEFAULT);
		
		for (DamageUIInfo ui : damageList_) {
			//スクリーン座標計算して、画面内かどうか
			XMFLOAT3 pos = Camera::CalcScreenPosition(ui.wPos);
			if (!Camera::IsScreenPositionWithinScreen(pos)) continue;

			//座標変換して表示
			pos.x = (pos.x + 1.0f) * 0.5f;
			pos.y = 1.0f - ((pos.y + 1.0f) * 0.5f);
			pos.x *= Direct3D::screenWidth_;
			pos.y *= Direct3D::screenHeight_;
			pText_->SetAlpha(ui.alpha);
			pText_->Draw((int)pos.x, (int)pos.y, ui.damage);
		}
	}

}