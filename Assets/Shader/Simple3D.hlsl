//������������������������������������������������������������������������������
 // �e�N�X�`�����T���v���[�f�[�^�̃O���[�o���ϐ���`
//������������������������������������������������������������������������������
Texture2D       g_texture : register(t0);           //�e�N�X�`���[
SamplerState    g_sampler : register(s0);           //�T���v���[
SamplerState	g_texDepthSampler : register(s1);   //�T���v���[
Texture2D       g_texDepth : register(t2);          //�[�x�e�N�X�`���[

//������������������������������������������������������������������������������
// �R���X�^���g�o�b�t�@
// DirectX �����瑗�M����Ă���A�|���S�����_�ȊO�̏����̒�`
//������������������������������������������������������������������������������
cbuffer global
{
    float4x4 matWVP;        //���[���h�E�r���[�E�v���W�F�N�V�����̍����s��
    float4x4 matNormal;     //�@���̕ϊ��s��i��]�s��Ɗg��̋t�s��j
    float4x4 matWorld;      //���[���h�ϊ��s��
    float4x4 mWLPT;         //���[���h�E���C�g�r���[�E�v���W�F�N�V�����EUV �s��̍���
    float4 diffuseColor;    //�}�e���A���̐F
    float4 speculer;        //�X�y�L�����[�J���[
    float4 camPos;          //�J�����̍��W
    float4 lightPos;        //���C�g�̍��W
    float shininess;        //�n�C���C�g�̋���
    bool isTexture;         //�e�N�X�`�������邩
};

//������������������������������������������������������������������������������
// ���_�V�F�[�_�[�o�́��s�N�Z���V�F�[�_�[���̓f�[�^�\����
//������������������������������������������������������������������������������
struct VS_OUT
{
    float4 pos : SV_POSITION; //�ʒu
    float2 uv : TEXCOORD; //UV���W
    float4 color : COLOR; //�F�i���邳�j
    float4 LightTexCoord : TEXCOORD5;
    float4 LighViewPos : TEXCOORD6;
};

//������������������������������������������������������������������������������
// ���_�V�F�[�_
//������������������������������������������������������������������������������
VS_OUT VS(float4 pos : POSITION, float4 uv : TEXCOORD, float4 normal : NORMAL)
{
	//�s�N�Z���V�F�[�_�[�֓n�����
    VS_OUT outData;

	//���[�J�����W�ɁA���[���h�E�r���[�E�v���W�F�N�V�����s���������
	//�X�N���[�����W�ɕϊ����A�s�N�Z���V�F�[�_�[��
    outData.pos = mul(pos, matWVP);
    outData.uv = uv;

	//�@������]
    normal = mul(normal, matNormal);
    normal = normalize(normal);

    float4 light = normalize(lightPos - mul(pos, matWorld));
    outData.color = clamp(dot(normal, light), 0, 1);
    
    //���C�g�r���[���Q�Ƃ���Ƃ��A�肪����ƂȂ�e�N�X�`���[���W 
    outData.LightTexCoord = mul(pos, mWLPT); //���̓_���A���C�g�r���[�ł������Ƃ��̈ʒu���킩�� 

	//���C�g�r���[�ɂ�����ʒu(�ϊ���) 
    outData.LighViewPos = mul(pos, matWorld);

	//�܂Ƃ߂ďo��
    return outData;
}

//������������������������������������������������������������������������������
// �s�N�Z���V�F�[�_
//������������������������������������������������������������������������������
float4 PS(VS_OUT inData) : SV_Target
{
    float4 diffuse;
    float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    if (isTexture == true)
    {
		//�e�N�X�`���̐F
        diffuse = g_texture.Sample(g_sampler, inData.uv);
    }
    else
    {
		//�}�e���A���̐F
        diffuse = diffuseColor * inData.color;
        diffuse.a = 1;
    }

	//�e�̏��� 
    inData.LightTexCoord /= inData.LightTexCoord.w;
    float TexValue = g_texDepth.Sample(g_texDepthSampler, inData.LightTexCoord.xy).r;
    float LightLength = inData.LighViewPos.z / inData.LighViewPos.w;
    //LightLength = length(inData.LighViewPos - lightPos) / 30.0f;
    
    if (TexValue + 0.005 < LightLength)  //���C�g�r���[�ł̒������Z���i���C�g�r���[�ł͎Օ���������j 
    {
        //diffuse *= 0.3f;  
    }
    diffuse.a = 1.0f;
    return diffuse;
    
    return diffuse + ambient;
	
}