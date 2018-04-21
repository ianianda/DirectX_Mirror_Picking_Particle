#ifndef _EPPARTICLESYSTEM_H_
#define _EPPARTICLESYSTEM_H_

#include <d3d9.h>
#include <d3dx9.h>

struct EpParticle
{
	D3DXVECTOR3 pos;
	float psize;
	D3DCOLORVALUE color;
	D3DXVECTOR3	velo;
};

struct stParticleVertex
{
	D3DVECTOR pos;
	float psize;
	D3DCOLOR color;
};

#define D3DFVF_PARTICLE (D3DFVF_XYZ | D3DFVF_PSIZE | D3DFVF_DIFFUSE)

inline float rangeRand(float min, float max);
inline DWORD FtoDW(float val) { return *((DWORD*)&val); }

class EpParticleSystem
{
public:
	EpParticleSystem(D3DVECTOR position, D3DVECTOR range, D3DVECTOR acceleration,
		D3DVECTOR emitterPositionMin, D3DVECTOR emitterPositionMax,
		D3DVECTOR velocityMin, D3DVECTOR velocityMax,
		D3DCOLORVALUE colorMin, D3DCOLORVALUE colorMax,
		float psizeMin, float psizeMax,
		int maxCount, int emitCount, float emitInterval,
		LPDIRECT3DDEVICE9 device, LPDIRECT3DTEXTURE9 texture);
	virtual ~EpParticleSystem();
	bool Initialize();
	void Update(float delta);
	bool Render();

protected:
	void emit(int emitCount);

protected:
	D3DXVECTOR3 m_Pos;		
	D3DXVECTOR3 m_Range;	
	D3DXVECTOR3 m_Accel;	
	D3DXVECTOR3 m_EmiPosMin, m_EmiPosMax;	
	D3DXVECTOR3 m_VeloMin, m_VeloMax;		
	D3DCOLORVALUE m_ColorMin, m_ColorMax;	
	float m_PSizeMin, m_PSizeMax;			
	int m_Count, m_MaxCount;		
	int m_EmiCount; float m_EmiInterval;			
	float m_Interval;			

	EpParticle *m_Particles;		

	LPDIRECT3DDEVICE9 m_pDevice;	
	LPDIRECT3DTEXTURE9 m_pTexture;	
	LPDIRECT3DVERTEXBUFFER9 m_pVB;	

	D3DXMATRIX m_WorldMat;		
};
#endif