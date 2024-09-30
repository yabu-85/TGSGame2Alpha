#include "DamageUI.h"
#include "../Engine/Transform.h"
#include "../Engine/Direct3D.h"
#include "../Engine/Camera.h"
#include "../Engine/Text.h"
#include "../Engine/Image.h"
#include "../Engine/Global.h"
#include "../Other/GameManager.h"
#include "../Player/Player.h"
#include <vector>

namespace {
	const int	ALPHA_REDUCE = 25;			//�����x������l
	const float TIME_REDUCE = 0.03f;		//Time������l
	const float KILL_THRESHOLD = 0.2f;		//�����鎞�Ԃ̂������l

	//DamageText
	const float RIZE_VALUE = 0.02f;			//�㏸�l
	const float RANDOM_POS_MAX = 0.3f;		//0�`X�̃����_���Ȓl�̍ő�l

	const XMFLOAT3 ONE_TEXT_SCALE = XMFLOAT3(0.7f, 0.7f, 1.0f);	//�������������T�C�Y
	const XMFLOAT3 TWO_TEXT_SCALE = XMFLOAT3(1.0f, 0.7f, 1.0f);	//�������蕶���T�C�Y
	const float TEXT_POSITION_Y = 0.4f;							//�����|�W�V����

	//DirectionDamage
	const XMFLOAT3 DIRECTION_SCALE = XMFLOAT3(0.3f, 0.3f, 1.0f);

}

struct DamageUIInfo {
	int damage;
	int alpha;
	float time;
	XMFLOAT3 wPos;
};

struct DirectionDamageUIInfo {
	XMFLOAT3 pos1, pos2;
	int alpha;
	float time;
};

namespace DamageUI {
	std::vector<DamageUIInfo> damageList_[2];				//�v���C���[�̐������X�g����
	std::vector<DirectionDamageUIInfo> directionDamage_[2];	//�v���C���[�̐������X�g����

	Text* pText_ = nullptr;
	int hPict_;

	void Initialize()
	{
		pText_ = new Text();
		pText_->Initialize();
		if(GameManager::IsOnePlayer()) pText_->SetScale(ONE_TEXT_SCALE);
		else pText_->SetScale(TWO_TEXT_SCALE);

		const char* fileName[] = { "Image/directionDamage.png" };
		hPict_ = Image::Load(fileName[0]);
		assert(hPict_ >= 0);

		DirectionDamageUIInfo info;
		directionDamage_[0].push_back(info);
		directionDamage_[1].push_back(info);
	}

	void SceneChange()
	{
		pText_ = new Text();
		pText_->Initialize();
		if (GameManager::IsOnePlayer()) pText_->SetScale(ONE_TEXT_SCALE);
		else pText_->SetScale(TWO_TEXT_SCALE);

		const char* fileName[] = { "Image/directionDamage.png" };
		hPict_ = Image::Load(fileName[0]);
		assert(hPict_ >= 0);

		ResetDamageList();
	}

	void Release()
	{
	}

	void AddDirectionDamage(XMFLOAT3 pos, XMFLOAT3 pos2, int index)
	{
		DirectionDamageUIInfo info;
		info.alpha = 255;
		info.time = 1.0f;
		info.pos1 = pos;
		info.pos2 = pos2;
		directionDamage_[index].at(0) = (info);
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
			};

			for (auto it = directionDamage_[i].begin(); it != directionDamage_[i].end();) {
				(*it).time -= TIME_REDUCE;

				//������A�j���[�V����
				if ((*it).time <= KILL_THRESHOLD) {
					(*it).alpha -= ALPHA_REDUCE;
				}

				//�폜
				if ((*it).time <= 0.0f) {
				//	it = directionDamage_[i].erase(it);
				//	continue;
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

		if (!GameManager::GetPlayer(index)) return;

		for (DirectionDamageUIInfo ui : directionDamage_[index]) {
			//���W�ϊ����ĕ\��
			float playerRotateY = GameManager::GetPlayer(index)->GetRotate().y;
			XMFLOAT2 radius = XMFLOAT2(0.5f, 0.6f);
			XMFLOAT3 vec = Float3Normalize(Float3Sub(ui.pos1, ui.pos2));
			XMFLOAT3 rotate = CalculationRotateXY(vec);
			rotate.y -= playerRotateY;
			float angleRadians = XMConvertToRadians((rotate.y + 90.0f) * -1.0f);

			//�ȉ~����̍��W���v�Z
			Transform t;
			t.position_.x = radius.x * cosf(angleRadians);
			t.position_.y = radius.y * sinf(angleRadians);
			t.scale_ = DIRECTION_SCALE;
			t.rotate_.z = (rotate.y + 180.0f) * -1.0f;
			if (rotate.x > 0) t.rotate_.x = (-rotate.x) + 60.0f;
			else t.rotate_.x = (rotate.x) + 50.0f;

			Image::SetAlpha(hPict_, ui.alpha);
			Image::SetTransform(hPict_, t);
			Image::Draw(hPict_);
		}

	}

}