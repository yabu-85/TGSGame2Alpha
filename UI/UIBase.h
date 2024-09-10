#pragma once
#include <DirectXMath.h>
#include <string>
#include <functional>
#include "../Engine/Transform.h"

using namespace DirectX;

class UIBase
{
protected:
	bool isBound_;						//�͈͂̓����ɂ��邩�ǂ���
	bool isSelect_;						//�R���g���[���[�p�I�����Ă��邩
	bool selectPlus_;					//�A�j���[�V�����v�Z�p
	bool screenSelectPossible_;			//�X�N���[����ʂŃZ���N�g��ϓ������Ă�����

	int hButtonPict_[2];				//UI�{�̂̉摜�n���h��
	int hImagePict_;					//�\������摜�̃n���h��
	int alpha_;							//�����x
	float selectAnim_;					//�I�𒆂̃A�j���[�V�����O�`�P

	Transform frameTransform_;			//UI���̂�Transform
	Transform imageTransform_;			//�\������Image�Ƃ���Transform
	std::function<void()> onClick_;		//UI����������ĂԊ֐�

	//�I�𒆂�selectAnim�̌v�Z
	void SelectAnimUpdate();			

public:
	UIBase(XMFLOAT2 pos, XMFLOAT2 size, std::function<void()> onClick, XMFLOAT2 tsize);
	virtual ~UIBase();
	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual void Initialize(std::string name) = 0;
	virtual bool IsWithinBound() = 0;
	virtual void SelectUpdate() = 0;
	virtual void OnClick();

	XMFLOAT3 GetPosition() { return frameTransform_.position_; }

	bool GetBound() { return isBound_; }
	bool GetSelect() { return isSelect_; }
	bool GetSelectPossible() { return screenSelectPossible_; }

	void SetBound(bool b) { isBound_ = b; }
	void SetSelect(bool b) { isSelect_ = b; }

	/// <summary>
	/// UI�̐���
	/// Slider�̖��O�w��͌��ݎ������ĂȂ�����A�������Ă�����
	/// Button�͓ǂݍ��ޕ����摜�̖��O�ɂ��邱��
	/// </summary>
	template<class T>
	inline T* UIInstantiate(std::string name, XMFLOAT2 pos, XMFLOAT2 size, std::function<void()> onClick)
	{
		return UIInstantiate<T>(name, pos, size, XMFLOAT2(1.0f, 1.0f), onClick);
	}
	template<class T>
	inline T* UIInstantiate(std::string name, XMFLOAT2 pos, XMFLOAT2 size, XMFLOAT2 tsize, std::function<void()> onClick)
	{
		T* ui = new T(pos, size, onClick, tsize);
		ui->Initialize(name);
		return ui;
	}

};

