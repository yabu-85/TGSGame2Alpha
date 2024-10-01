#pragma once
#include <vector>
class OneFrame;
class GameObject;

struct AnimData {
	int startFrame;
	int endFrame;
	std::vector<OneFrame*> frameList;
};

struct BlendData {
	float currentBlend;	//����Blend�̒l�i1�`0�j
	float decreaseBlend;	//1�t���[���Ńu�����h�l���炷��
};

class AnimationController
{
	int modelNum_;						//�S�����Ă郂�f���ԍ�
	int currentAnim_;					//���Đ����Ă���Animation��Id
	GameObject* pGameObject_;			//�e
	std::vector<AnimData> animDatas_;	//�S�Ẵf�[�^��ۑ�
	std::vector<BlendData> blendDatas_;	//�S�Ẵf�[�^��ۑ�

public:
	AnimationController(int number, GameObject* obj);
	~AnimationController();
	void Update();
	void AddAnim(int s, int e);

	//�A�j���[�V�����ʒm�̒ǉ�
	void AddAnimNotify(int animId, OneFrame* action);

	/// <summary>
	/// �u�����h���̒ǉ�
	/// </summary>
	/// <param name="id">Animation��ID</param>
	/// <param name="factor">�u�����h�l</param>
	/// <param name="decrease">1�t���[���Ńu�����h�l���炷��</param>
	void AddBlend(int id, float factor, float decrease);

	/// <summary>
	/// ���̃A�j���[�V������ݒ�
	/// </summary>
	/// <param name="id">Animation��ID</param>
	/// <param name="speed">�Đ����x</param>
	/// <param name="addStart">�o�^���ꂽStart�t���[���ɉ��Z����l</param>
	/// <param name="addEnd">�o�^���ꂽEnd�t���[���Ɍ��Z����l</param>
	void SetNextAnim(int id, float speed = 1.0f, int addStart = 0, int addEnd = 0);

	AnimData GetAnim(int id);
	int GetAnimTime(int id);
	int GetCurrentAnim() { return currentAnim_; }
};
