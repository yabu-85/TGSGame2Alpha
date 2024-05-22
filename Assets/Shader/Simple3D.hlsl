//������������������������������������������������������������������������������
 // �e�N�X�`�����T���v���[�f�[�^�̃O���[�o���ϐ���`
//������������������������������������������������������������������������������
Texture2D       g_texture : register(t0);           //�e�N�X�`���[
SamplerState    g_sampler : register(s0);           //�T���v���[
SamplerState	g_depthSampler : register(s1);   //�T���v���[
Texture2D       g_depthTexture : register(t2); //�[�x�e�N�X�`���[

//������������������������������������������������������������������������������
// �R���X�^���g�o�b�t�@
// DirectX �����瑗�M����Ă���A�|���S�����_�ȊO�̏����̒�`
//������������������������������������������������������������������������������
cbuffer global
{
    float4x4 matWVP;        //���[���h�E�r���[�E�v���W�F�N�V�����̍����s��
    float4x4 matNormal;     //�@���̕ϊ��s��i��]�s��Ɗg��̋t�s��j
    float4x4 matWorld;      //���[���h�ϊ��s��
    float4x4 g_mWLP;		//���[���h�E�h���C�g�r���[�h�E�v���W�F�N�V�����̍��� 
	float4x4 g_mWLPT;	    //���[���h�E�h���C�g�r���[�h�E�v���W�F�N�V�����EUV �s��̍��� 
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
    float4 pos : SV_POSITION;       //�ʒu
    float2 uv : TEXCOORD0;          //UV���W
	float4 normal : TEXCOORD1;		//�@��
    float4 color : COLOR;           //�F�i���邳�j
    float4 lightDir : TEXCOORD5;    //���C�g�̕���
	float4 eye	  : TEXCOORD2;		//����
    float4 lightTex : TEXCOORD3;    //���C�g���猩���g�L�̃e�N�X�`���_�ʒu
    float4 lightViewPos : TEXCOORD4;//���C�g���猩�����_���W
    
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
    outData.uv = uv.xy;

	//�@������]
    normal.w = 0;
    normal = mul(normal, matNormal);
    outData.normal = normal;

    //�����x�N�g���i�n�C���C�g�̌v�Z�ɕK�v
    float4 worldPos = mul(pos, matWorld);           //���[�J�����W�Ƀ��[���h�s��������ă��[���h���W��
    outData.eye = normalize(camPos - worldPos);     //���_���璸�_�ʒu�������Z�����������߂ăs�N�Z���V�F�[�_�[��
    
    float4 light = normalize(mul(pos, matWorld) - lightPos);
    outData.lightDir = light;
    outData.color = clamp(dot(normal, light), 0, 1);
    
    outData.lightTex = mul(pos, g_mWLPT);
    outData.lightViewPos = mul(pos, g_mWLP);
    
	//�܂Ƃ߂ďo��
    return outData;
}

//������������������������������������������������������������������������������
// �s�N�Z���V�F�[�_
//������������������������������������������������������������������������������
float4 PS(VS_OUT inData) : SV_Target
{
    //���C�g�̌���
    float4 lightDir = normalize(inData.lightDir);

	//�@���̓s�N�Z���V�F�[�_�[�Ɏ����Ă������_�ŕ⊮���꒷�����ς���Ă���
	//���K�����Ă����Ȃ��Ɩʂ̖��邳�����������Ȃ�
    inData.normal.w = 0;
    inData.normal = normalize(inData.normal);

    float4 shade = saturate(dot(-lightDir, inData.normal));
    shade.a = 1;
    
    float4 diffuse;
	//�e�N�X�`���L��
    if (isTexture == true)
    {
		//�e�N�X�`���̐F
        diffuse = g_texture.Sample(g_sampler, inData.uv);
    }
    else
    {
		//�}�e���A���̐F
        diffuse = diffuseColor;
    }

	//�����i�A���r�G���g�j
	//�����Maya���Ŏw�肵�A�O���[�o���ϐ��Ŏ󂯎�������̂����̂܂�
    float4 ambient = float4(0.2f, 0.2f, 0.2f, 1.0f);

	//���ʔ��ˌ��i�X�y�L�����[�j
    float4 speculer = float4(0.0f, 0.0f, 0.0f, 1.0f); //�Ƃ肠�����n�C���C�g�͖����ɂ��Ă����āc
    float4 specColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
    
    float4 R = reflect(lightDir, inData.normal); //�����˃x�N�g��
    speculer = pow(saturate(dot(R, inData.eye)), shininess) * specColor; //�n�C���C�g�����߂�

	//�ŏI�I�ȐF
    float4 color = diffuse * shade + diffuse * ambient + speculer;
    
    inData.lightTex /= inData.lightTex.w;
    float TexValue = g_depthTexture.Sample(g_depthSampler, inData.lightTex.xy).r;
    float LightLength = inData.lightViewPos.z / inData.lightViewPos.w;
    
	//���C�g���猩�����_��Z�l�Ɛ[�x�e�N�X�`���̒l���ׂāA�[�x�e�N�X�`���̕�����������Ήe�Ƃ݂Ȃ�
    if (TexValue + 0.003f < LightLength)
    {
        //color.r = 2.0f;
        color *= 0.3f;
    }

	//�����A���t�@�l���������ł������łȂ����
    if (diffuse.a == 1.0f) color.a = 1.0f;
    else color.a = diffuse.a;

    return color;
}