// ���_�V�F�[�_�[����s�N�Z���V�F�[�_�[�ւ̂����Ɏg�p����\����
struct Output
{
    // �V�X�e���p���_���W
	float4 svpos : SV_POSITION;
    // uv�l
	float2 uv : TEXCOORD;
};

// 0�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
Texture2D<float4> tex : register(t0);
// 0�ԃX���b�g�ɐݒ肳�ꂽ�T���v���[
SamplerState smp : register(s0);

// �萔�o�b�t�@
cbuffer cbuff0 : register(b0)
{
	matrix mat;
};