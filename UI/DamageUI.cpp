#include "DamageUI.h"
#include "../Engine/Transform.h"
#include "../Engine/Direct3D.h"
#include "../Engine/Camera.h"
#include "../Engine/Text.h"
#include <vector>

namespace {
	const XMFLOAT3 TEXT_SCALE = XMFLOAT3(0.7f, 0.7f, 1.0f);	//文字サイズ
	const XMFLOAT2 TEXT_POSITION = XMFLOAT2(0.3f, 0.4f);	//文字ポジション
	const int	ALPHA_REDUCE = 25;							//透明度下げる値
	const float TIME_REDUCE = 0.03f;						//Time下げる値
	const float RANDOM_POS_MAX = 0.3f;						//0～Xのランダムな値の最大値

	//消える時のアニメーション
	const float KILL_THRESHOLD = 0.2f;	//消える時間のしきい値
	const float RIZE_VALUE = 0.02f;		//上昇値

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
		pText_ = new Text();
		pText_->Initialize();
		pText_->SetScale(TEXT_SCALE);
	}

	void SceneChange()
	{
		pText_ = new Text();
		pText_->Initialize();
		pText_->SetScale(TEXT_SCALE);
	}

	void Release()
	{

	}

	void AddDamage(XMFLOAT3 _pos, int _damage)
	{
		DamageUIInfo ui;
		ui.damage = _damage;
		ui.alpha = 255;
		ui.time = 1.0f;

		//ポジションずらして設定
		ui.wPos = _pos;
		ui.wPos.x += (float)std::rand() / RAND_MAX * RANDOM_POS_MAX;
		ui.wPos.y += (float)std::rand() / RAND_MAX * RANDOM_POS_MAX;
		damageList_.push_back(ui);
	}

	void ResetDamageList()
	{
		damageList_.clear();
	}

	void Update()
	{
		for (auto it = damageList_.begin(); it != damageList_.end();) {
			(*it).time -= TIME_REDUCE;

			//消えるアニメーション
			if ((*it).time <= KILL_THRESHOLD) {
				(*it).wPos.y += RIZE_VALUE;
				(*it).alpha -= ALPHA_REDUCE;
			}

			//削除
			if ((*it).time <= 0.0f) {
				it = damageList_.erase(it);
				continue;
			}
			++it;
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