Texture2D g_texture : register(t0); //�e�N�X�`���[
SamplerState g_sampler : register(s0); //�T���v���[
Texture2D g_textureNormal : register(t1); //�e�N�X�`���[

//������������������������������������������������������������������������������
// �R���X�^���g�o�b�t�@
// DirectX �����瑗�M����Ă���A�|���S�����_�ȊO�̏����̒�`
//������������������������������������������������������������������������������
cbuffer global
{
    float4x4 matWVP;
    float4x4 matNormal;
    float4x4 matWorld;
    float4x4 g_mWLP; //���[���h�E�h���C�g�r���[�h�E�v���W�F�N�V�����̍��� 
    float4x4 g_mWLPT; //���[���h�E�h���C�g�r���[�h�E�v���W�F�N�V�����EUV �s��̍���
    float4 diffuseColor;
    float4 speculer;
    float4 camPos;
};

//������������������������������������������������������������������������������
// ���_�V�F�[�_�[�o�́��s�N�Z���V�F�[�_�[���̓f�[�^�\����
//������������������������������������������������������������������������������
struct VS_OUT
{
    float4 pos : SV_POSITION; //�ʒu
    float z : TEXCOORD1;
};

//������������������������������������������������������������������������������
// ���_�V�F�[�_
//������������������������������������������������������������������������������
VS_OUT VS(float4 pos : POSITION)
{
    VS_OUT outData;
    outData.pos = mul(pos, matWVP);
    outData.z = length(camPos - mul(pos, matWorld)) / 30.0f;
    return outData;
}

//������������������������������������������������������������������������������
// �s�N�Z���V�F�[�_
//������������������������������������������������������������������������������
float4 PS(VS_OUT inData) : SV_Target
{
    return float4(inData.z, inData.z, inData.z, 1.0f);
}
