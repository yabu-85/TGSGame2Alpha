#pragma once

//�C���N���[�h
#include <Windows.h>
#include <d3d11.h>
#include <DirectXMath.h>

//�����J
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

using namespace DirectX;
#define PC_CTRL 1
#define ONE_PLAYER 1

//-----------------------------------------------------------
//��ʂ̕`��Ɋւ��鏈��
//-----------------------------------------------------------
namespace Direct3D
{
	////////////////////////�O��������A�N�Z�X����ϐ��Q///////////////////////////////
	//�`����s�����߂̊��⃊�\�[�X�̍쐬�Ɏg��
	extern ID3D11Device*           pDevice_;

	//GPU�ɖ��߂��o�����߂̂��
	extern ID3D11DeviceContext*    pContext_;
	
	//�V���h�E�}�b�v�p
	extern ID3D11ShaderResourceView* pDepthSRV_;
	extern ID3D11SamplerState* pDepthSampler_;
	extern XMMATRIX lightViewMatrix;
	extern XMMATRIX clipToUVMatrix;

	//���V�F�[�_�[�֘A�ŕK�v�ȃZ�b�g
	enum SHADER_TYPE{SHADER_3D, SHADER_2D, SHADER_UNLIT, SHADER_BILLBOARD, SHADER_SHADOWMAP, SHADER_MAX};	//3�^�C�v�i3D�p�A2D�p�A�����蔻��g�\���p�j
	struct SHADER_BUNDLE
	{
		//�y���_���̓��C�A�E�g���z
		//1�̒��_�f�[�^���ǂ�ȏ����ǂ�ȏ��ԂŊi�[���Ă邩�i�ʒu�ƐF�Ɩ@���Ɓc�Ƃ��j
		ID3D11InputLayout *pVertexLayout;

		//�y���_�V�F�[�_�z
		//�V�F�[�_�[�i�Z�Z.hlsl�j�̒��̒��_�V�F�[�_�[�iVS�j�������R���p�C���������̂�����
		//�V�F�[�_�[�̓n�[�h�ɂ���ē��삪�قȂ�̂ŁA���s���ɃR���p�C������B
		ID3D11VertexShader *pVertexShader;

		//�y�s�N�Z���V�F�[�_�z
		//�V�F�[�_�[�i�Z�Z.hlsl�j�̒��̃s�N�Z���V�F�[�_�[�iPS�j�������R���p�C���������̂�����
		ID3D11PixelShader *pPixelShader;

		//�y���X�^���C�U�z
		//���_�̕\���ʒu�m���A��ʂ̂ǂ̃s�N�Z�������点��΂��������߂����
		ID3D11RasterizerState*	pRasterizerState;
	};

	//���u�����h���[�h
	enum BLEND_MODE
	{
		BLEND_DEFAULT, BLEND_ADD, BLEND_MAX
	};

	//���̑�
	extern int		screenWidth_;			//�X�N���[���̕�
	extern int		screenHeight_;			//�X�N���[���̍���
	extern bool		isDrawCollision_;		//�R���W������\�����邩�t���O
	extern float	emphasisTime_;			//�����\���A�e�ۂɓ����������p��

	extern XMFLOAT3	PlayerPosition;
	extern XMFLOAT3	EnemyPosition;
	extern float	playerSpeed;

	extern bool		playerClimb;
	extern bool		playerFaly;

	////////////////////////��������͊֐�///////////////////////////////

	//����������
	//�����FhWnd			�E�B���h�E�n���h��
	//�����FscreenWidth		�X�N���[���̕�
	//�����FscreenHeight	�X�N���[���̍���
	HRESULT Initialize(HWND hWnd, int screenWidth, int screenHeight);

	//�V�F�[�_�[�֘A�ŕK�v�ȃZ�b�g����
	void InitShaderBundle();

	//������`�悷��ShaderBundle��ݒ�
	//�����Ftype	SHADER_3D, SHADER_2D, SHADER_UNLIT�̂ǂꂩ
	void SetShader(SHADER_TYPE type);
	SHADER_TYPE GetCurrentShader();

	//�u�����h���[�h�̕ύX
	//�����FblendMode	BLEND_DEFAULT	�ʏ�
	//					BLEND_ADD		���Z�����i�p�[�e�B�N���p�j
	void SetBlendMode(BLEND_MODE blendMode);

	//�`��J�n
	void BeginDraw();
	void BeginDraw2();

	void SetViewPort(int i);
	void SetViewOne();	//��l�p�̐ݒ�
	void SetViewTwo();	//��l�p�̐ݒ�

	//�`��I��
	void EndDraw();

	//�J������
	void Release();


	//�O�p�`�Ɛ����i���C�j�̏Փ˔���i�Փ˔���Ɏg�p�j
	//�����Fstart�@		���C�̃X�^�[�g�ʒu
	//�����Fdirection	���C�̕���
	//�����Fv0,v1,v2	�O�p�`�̊e���_�ʒu
	//�����Fdistance	�Փ˓_�܂ł̋�����Ԃ�
	//�ߒl�F�Փ˂������ǂ���
	bool Intersect(XMFLOAT3& start, XMFLOAT3& direction, XMFLOAT3& v0, XMFLOAT3& v1, XMFLOAT3& v2, float* distance, XMVECTOR* normal);

	//XMVECTOR��
	bool Intersect(XMFLOAT3& start, XMFLOAT3& direction, XMVECTOR& v0, XMVECTOR& v1, XMVECTOR& v2, float* distance, XMVECTOR* normal);

	//Z�o�b�t�@�ւ̏�������ON/OFF
	//�����FisWrite	  true=��������ON�^false=��������OFF
	void SetDepthBafferWriteEnable(bool isWrite);
};

