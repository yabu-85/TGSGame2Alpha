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
	int modelNum_;						//担当してるモデル番号
	int currentAnim_;					//今再生しているAnimationのId
	GameObject* pGameObject_;			//親
	std::vector<AnimData> animDatas_;	//全てのデータを保存

public:
	AnimationController(int number, GameObject* obj);
	~AnimationController();
	void Update();
	void AddAnim(int s, int e);

	//アニメーション通知の追加
	void AddAnimNotify(int animId, OneFrame* action);

	/// <summary>
	/// 次のアニメーションを設定
	/// </summary>
	/// <param name="id">AnimationのID</param>
	/// <param name="speed">再生速度</param>
	/// <param name="addStart">登録されたStartフレームに加算する値</param>
	/// <param name="addEnd">登録されたEndフレームに減算する値</param>
	void SetNextAnim(int id, float speed = 1.0f, int addStart = 0, int addEnd = 0);

	AnimData GetAnim(int id);
	int GetAnimTime(int id);
	int GetCurrentAnim() { return currentAnim_; }
};
