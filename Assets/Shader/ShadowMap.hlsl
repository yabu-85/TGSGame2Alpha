//������������������������������������������������������������������������������
// �R���X�^���g�o�b�t�@
// DirectX �����瑗�M����Ă���A�|���S�����_�ȊO�̏����̒�`
//������������������������������������������������������������������������������
cbuffer global
{
    float4x4 matWVP;
    float4x4 matNormal;
    float4x4 matWorld;
    float4x4 g_mWLPT;
    float4 diffuseColor;
    float4 speculer;
    float4 camPos;
    float4 lightPos;
};

//������������������������������������������������������������������������������
// ���_�V�F�[�_�[�o�́��s�N�Z���V�F�[�_�[���̓f�[�^�\����
//������������������������������������������������������������������������������
struct VS_OUT
{
    float4 pos : SV_POSITION; //�ʒu
    float depth : TEXCOORD1;
};

//������������������������������������������������������������������������������
// ���_�V�F�[�_
//������������������������������������������������������������������������������
VS_OUT VS(float4 pos : POSITION)
{
    VS_OUT outData;
    outData.pos = mul(pos, matWVP);
    outData.depth = length(lightPos - mul(pos, matWVP)) / 30.0f;
    return outData;
}

//������������������������������������������������������������������������������
// �s�N�Z���V�F�[�_
//������������������������������������������������������������������������������
float4 PS(VS_OUT inData) : SV_Target
{
    return float4(inData.depth, inData.depth, inData.depth, 1.0f);
}
