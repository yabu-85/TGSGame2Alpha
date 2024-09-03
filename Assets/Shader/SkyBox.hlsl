//������������������������������������������������������������������������������
 // �e�N�X�`�����T���v���[�f�[�^�̃O���[�o���ϐ���`
//������������������������������������������������������������������������������
Texture2D		g_texture: register(t0);	//�e�N�X�`���[
SamplerState	g_sampler : register(s0);	//�T���v���[

//������������������������������������������������������������������������������
 // �R���X�^���g�o�b�t�@
// DirectX �����瑗�M����Ă���A�|���S�����_�ȊO�̏����̒�`
//������������������������������������������������������������������������������
cbuffer global
{
    float4x4 matWVP; //���[���h�E�r���[�E�v���W�F�N�V�����̍����s��
    float4x4 matNormal; //�@���̕ϊ��s��i��]�s��Ɗg��̋t�s��j
    float4x4 matWorld; //���[���h�ϊ��s��
    float4x4 g_mWLP; //���[���h�E���C�g�r���[�E�v���W�F�N�V�����̍��� 
    float4x4 g_mWLPT; //���[���h�E���C�g�r���[�E�v���W�F�N�V�����EUV �s��̍��� 
    float4 diffuseColor; //�}�e���A���̐F
    float4 speculer; //�X�y�L�����[�J���[
    float4 camPos; //�J�����̍��W
    float4 lightPos; //���C�g�̍��W
    float shininess; //�n�C���C�g�̋���
    bool isTexture; //�e�N�X�`�������邩
};

//������������������������������������������������������������������������������
// ���_�V�F�[�_�[�o�́��s�N�Z���V�F�[�_�[���̓f�[�^�\����
//������������������������������������������������������������������������������
struct VS_OUT
{
	float4 pos    : SV_POSITION;	//�ʒu
	float2 uv	  : TEXCOORD0;		//UV���W
};

//������������������������������������������������������������������������������
// ���_�V�F�[�_
//������������������������������������������������������������������������������
VS_OUT VS(float4 pos : POSITION, float4 Normal : NORMAL, float2 Uv : TEXCOORD)
{
	//�s�N�Z���V�F�[�_�[�֓n�����
    VS_OUT outData = (VS_OUT) 0;

    outData.pos = mul(pos, matWVP);
	outData.uv = Uv;

	//�܂Ƃ߂ďo��
	return outData;
}

//������������������������������������������������������������������������������
// �s�N�Z���V�F�[�_
//������������������������������������������������������������������������������
float4 PS(VS_OUT inData) : SV_Target
{
    if (isTexture == true) return g_texture.Sample(g_sampler, inData.uv);
	else return diffuseColor;
}