#pragma once
#include <DirectXMath.h>
using namespace DirectX;

//�萔
static const int LIGHT_TOTAL_NUM = 5;	//���C�g�̑���  

namespace Light
{
	void Initialize();
	void Draw();
	void SetPosition(int i, XMFLOAT4 pos);
	XMFLOAT4 GetPosition(int i);

	/// <summary>
	/// Player�̃��C�g�̈ʒu��ݒ�
	/// </summary>
	/// <param name="position">�ݒ肵�����ʒu</param>
	void SetPlayerPosition(XMFLOAT4 position);

	/// <summary>
	/// ���C�g�̈ʒu��ݒ�
	/// </summary>
	/// <param name="position">�ݒ肵�����ʒu</param>
	/// <param name="intensity">�ݒ肵��������</param>
	/// <returns>���C�g�̔ԍ�</returns>
	int CreateLight(XMFLOAT4 position, float intensity);

	/// <summary>
	/// �����ɂ���Ăǂ̂悤�ɖ��邳��ω������邩(�����p�����[�^)��ݒ�
	/// </summary>
	/// <param name="attenuation">�ݒ肵���������p�����[�^</param>
	void SetAttenuation(XMFLOAT4 attenuation);

	/// <summary>
	/// ���C�g�̌�����ݒ�
	/// </summary>
	/// <param name="direction">�ݒ肵�������C�g�̌���</param>
	void SetDirection(XMFLOAT4 direction);

	/// <summary>
	/// Player�̃��C�g�̋�����ݒ�
	/// </summary>
	/// <param name="intensity">�ݒ肵�������C�g�̋���</param>
	void SetPlayerIntensity(float intensity);

	/// <summary>
	/// ���C�g�̋�����ݒ�
	/// </summary>
	/// <param name="num">���C�g�̔ԍ�</param>
	/// <param name="intensity">���C�g�̋���</param>
	void SetIntensity(int num, float intensity);

	/// <summary>
	/// ���C�g�̍폜
	/// </summary>
	/// <param name="num">���C�g�̔ԍ�</param>
	void DeleteLight(int num);

	/// <summary>
	/// �ʒu���擾
	/// </summary>
	/// <param name="num">���C�g�̔ԍ�</param>
	/// <returns>�ʒu</returns>
	XMFLOAT4 GetPosition(int num);

	/// <summary>
	/// �����p�����[�^���擾
	/// </summary>
	/// <returns>�����p�����[�^</returns>
	XMFLOAT4 GetAttenuation();

	/// <summary>
	/// ���C�g�̌������擾
	/// </summary>
	/// <returns>���C�g�̌���</returns>
	XMFLOAT4 GetDirection();

	/// <summary>
	/// ���C�g�̋������擾
	/// </summary>
	/// <param name="num">���C�g�̔ԍ�</param>
	/// <returns>���C�g�̋���</returns>
	float  GetIntensity(int num);
};

