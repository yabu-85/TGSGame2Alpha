#pragma once
#include <vector>
class OneFrame;
class GameObject;

struct AnimData {
	int startFrame;
	int endFrame;
	std::vector<OneFrame*> frameList;
};

class AnimationController
{
	int modelNum_;						//�S�����Ă郂�f���ԍ�
	int currentAnim_;					//���Đ����Ă���Animation��Id
	GameObject* pGameObject_;			//�e
	std::vector<AnimData> animDatas_;	//�S�Ẵf�[�^��ۑ�

public:
	AnimationController(int number, GameObject* obj);
	~AnimationController();
	void Update();
	void AddAnim(int s, int e);

	//�A�j���[�V�����ʒm�̒ǉ�
	void AddAnimNotify(int animId, OneFrame* action);

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
