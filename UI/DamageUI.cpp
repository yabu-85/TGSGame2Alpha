#include "DamageUI.h"
#include "../Engine/Transform.h"
#include "../Engine/Direct3D.h"
#include "../Engine/Camera.h"
#include "../Engine/Text.h"
#include "../Other/GameManager.h"
#include <vector>

namespace {
	const XMFLOAT3 ONE_TEXT_SCALE = XMFLOAT3(0.7f, 0.7f, 1.0f);	//�������������T�C�Y
	const XMFLOAT3 TWO_TEXT_SCALE = XMFLOAT3(1.0f, 0.7f, 1.0f);	//�������蕶���T�C�Y
	const float TEXT_POSITION_Y = 0.4f;							//�����|�W�V����

	const int	ALPHA_REDUCE = 25;							//�����x������l
	const float TIME_REDUCE = 0.03f;						//Time������l
	const float RANDOM_POS_MAX = 0.3f;						//0�`X�̃����_���Ȓl�̍ő�l

	//�����鎞�̃A�j���[�V����
	const float KILL_THRESHOLD = 0.2f;	//�����鎞�Ԃ̂������l
	const float RIZE_VALUE = 0.02f;		//�㏸�l

}

struct DamageUIInfo {
	int damage;
	int alpha;
	float time;
	XMFLOAT3 wPos;
};

namespace DamageUI {
	std::vector<DamageUIInfo> damageList_[2];
	Text* pText_ = nullptr;

	void Initialize()
	{
		pText_ = new Text();
		pText_->Initialize();
		if(GameManager::IsOnePlayer()) pText_->SetScale(ONE_TEXT_SCALE);
		else pText_->SetScale(TWO_TEXT_SCALE);
	}

	void SceneChange()
	{
		pText_ = new Text();
		pText_->Initialize();
		if (GameManager::IsOnePlayer()) pText_->SetScale(ONE_TEXT_SCALE);
		else pText_->SetScale(TWO_TEXT_SCALE);

		ResetDamageList();
	}

	void Release()
	{

	}

	void AddDamage(XMFLOAT3 _pos, int _damage, int index)
	{
		DamageUIInfo ui;
		ui.damage = _damage;
		ui.alpha = 255;
		ui.time = 1.0f;

		//�|�W�V�������炵�Đݒ�
		ui.wPos = _pos;
		ui.wPos.y += TEXT_POSITION_Y;
		ui.wPos.x += (float)std::rand() / RAND_MAX * RANDOM_POS_MAX;
		ui.wPos.y += (float)std::rand() / RAND_MAX * RANDOM_POS_MAX;
		damageList_[index].push_back(ui);
	}

	void ResetDamageList()
	{
		for(int i = 0;i < 2;i++) damageList_[i].clear();

	}

	void Update()
	{
		for (int i = 0; i < 2; i++) {
			for (auto it = damageList_[i].begin(); it != damageList_[i].end();) {
				(*it).time -= TIME_REDUCE;

				//������A�j���[�V����
				if ((*it).time <= KILL_THRESHOLD) {
					(*it).wPos.y += RIZE_VALUE;
					(*it).alpha -= ALPHA_REDUCE;
				}

				//�폜
				if ((*it).time <= 0.0f) {
					it = damageList_[i].erase(it);
					continue;
				}
				++it;
			}
		}

	}

	void Draw(int index)
	{
		Direct3D::SetBlendMode(Direct3D::BLEND_DEFAULT);
		
		for (DamageUIInfo ui : damageList_[index]) {
			//�X�N���[�����W�v�Z���āA��ʓ����ǂ���
			XMFLOAT3 pos = Camera::CalcScreenPosition(ui.wPos);
			if (!Camera::IsScreenPositionWithinScreen(pos)) continue;

			//���W�ϊ����ĕ\��
			pos.x = (pos.x + 1.0f) * 0.5f;
			pos.y = 1.0f - ((pos.y + 1.0f) * 0.5f);
			pos.x *= Direct3D::screenWidth_;
			pos.y *= Direct3D::screenHeight_;
			pText_->SetAlpha(ui.alpha);
			pText_->Draw((int)pos.x, (int)pos.y, ui.damage);
		}
	}

}