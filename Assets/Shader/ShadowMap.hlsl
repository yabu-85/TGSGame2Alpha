//������������������������������������������������������������������������������
// �R���X�^���g�o�b�t�@
// DirectX �����瑗�M����Ă���A�|���S�����_�ȊO�̏����̒�`
//������������������������������������������������������������������������������
cbuffer global
{
    float4x4 matWVP; //���[���h�E�r���[�E�v���W�F�N�V�����̍����s��
    float4x4 matNormal; //�@���̕ϊ��s��i��]�s��Ɗg��̋t�s��j
    float4x4 matWorld; //���[���h�ϊ��s��
    float4x4 g_mWLP; //���[���h�E�h���C�g�r���[�h�E�v���W�F�N�V�����̍��� 
    float4x4 g_mWLPT; //���[���h�E�h���C�g�r���[�h�E�v���W�F�N�V�����EUV �s��̍��� 
    float4 diffuseColor; //�}�e���A���̐F
    float4 speculer; //�X�y�L�����[�J���[
    float4 camPos; //�J�����̍��W
    float4 lightPos; //���C�g�̍��W
};

//������������������������������������������������������������������������������
// ���_�V�F�[�_�[�o�́��s�N�Z���V�F�[�_�[���̓f�[�^�\����
//������������������������������������������������������������������������������
struct VS_OUT
{
    float4 pos : SV_POSITION; //�ʒu
    float4 depth : TEXCOORD1;
};

#define CALC_METHOD 1

//������������������������������������������������������������������������������
// ���_�V�F�[�_
//������������������������������������������������������������������������������
VS_OUT VS(float4 pos : POSITION)
{
    VS_OUT outData;
    
#if CALC_METHOD
    outData.pos = mul(pos, matWVP);
    outData.depth = outData.pos;
#else
    outData.pos = mul(pos, matWVP);
    outData.depth = length(camPos - mul(pos, matWorld)) / 40.0;
#endif
    
    return outData;
}

//������������������������������������������������������������������������������
// �s�N�Z���V�F�[�_
//������������������������������������������������������������������������������
float4 PS(VS_OUT inData) : SV_Target
{
#if CALC_METHOD
    float4 color = inData.depth.z / inData.depth.w;
    color.a = 1;
    return color;
#else    
    return float4(inData.depth.z, inData.depth.z, inData.depth.z, 1);
#endif
}
