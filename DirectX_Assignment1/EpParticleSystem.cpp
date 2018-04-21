#include "EpParticleSystem.h"
#include <stdlib.h>
#include <time.h>


EpParticleSystem::EpParticleSystem(D3DVECTOR position, D3DVECTOR range,
	D3DVECTOR acceleration, D3DVECTOR emitterPositionMin, D3DVECTOR emitterPositionMax,
	D3DVECTOR velocityMin, D3DVECTOR velocityMax, D3DCOLORVALUE colorMin,
	D3DCOLORVALUE colorMax, float psizeMin, float psizeMax,
	int maxCount, int emitCount, float emitInterval,
	LPDIRECT3DDEVICE9 device, LPDIRECT3DTEXTURE9 texture)
{
	m_Pos = position;
	m_Range = range;
	m_Accel = acceleration;
	m_EmiPosMin = emitterPositionMin;
	m_EmiPosMax = emitterPositionMax;
	m_VeloMin = velocityMin;
	m_VeloMax = velocityMax;
	m_ColorMin = colorMin;
	m_ColorMax = colorMax;
	m_PSizeMin = psizeMin;
	m_PSizeMax = psizeMax;
	m_Count = 0;
	m_MaxCount = maxCount;
	m_EmiCount = emitCount;
	m_EmiInterval = emitInterval;
	m_Interval = 0.0f;
	m_pDevice = device;
	m_pTexture = texture;
	m_Particles = NULL;
	m_pVB = NULL;
	srand((unsigned)time(NULL));
	D3DXMatrixTranslation(&m_WorldMat, m_Pos.x, m_Pos.y, m_Pos.z);
}

bool EpParticleSystem::Initialize()
{
	if (m_pDevice == NULL)
	{
		return false;
	}
	m_Particles = new EpParticle[m_MaxCount];
	if (m_Particles == NULL)
	{
		return false;
	}
	if (FAILED(m_pDevice->CreateVertexBuffer(sizeof(stParticleVertex) * m_MaxCount,
		D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY | D3DUSAGE_POINTS,
		D3DFVF_PARTICLE, D3DPOOL_DEFAULT, &m_pVB, NULL)))
		return false;

	return true;
}

void EpParticleSystem::Update(float delta)
{
	EpParticle *p = NULL;
	for (int i = 0; i < m_Count; i++)
	{
		p = &m_Particles[i];
		p->pos += p->velo;
		p->velo += m_Accel;

		if (!(p->pos.x <= m_Range.x / 2 && p->pos.x >= -m_Range.x / 2 &&
			p->pos.y <= m_Range.y / 2 && p->pos.y >= -m_Range.y / 2 &&
			p->pos.z <= m_Range.z / 2 && p->pos.z >= -m_Range.z / 2))
		{
			m_Particles[i] = m_Particles[m_Count - 1];
			m_Count--;	
			i--;	
		}
	}
	m_Interval += delta;
	if (m_Interval >= m_EmiInterval)
	{
		emit(m_EmiCount);
		m_Interval -= m_EmiInterval;
	}
}
float rangeRand(float min, float max)
{
	return (float)rand() / RAND_MAX * (max - min) + min;
}
void EpParticleSystem::emit(int emitCount)
{
	EpParticle *p = NULL;
	for (int i = 0; i < emitCount; i++)
	{
		if (m_Count == m_MaxCount) break;
		p = &m_Particles[m_Count];
		p->pos.x = rangeRand(m_EmiPosMin.x, m_EmiPosMax.x);
		p->pos.y = rangeRand(m_EmiPosMin.y, m_EmiPosMax.y);
		p->pos.z = rangeRand(m_EmiPosMin.z, m_EmiPosMax.z);
		p->velo.x = rangeRand(m_VeloMin.x, m_VeloMax.x);
		p->velo.y = rangeRand(m_VeloMin.y, m_VeloMax.y);
		p->velo.z = rangeRand(m_VeloMin.z, m_VeloMax.z);
		p->psize = rangeRand(m_PSizeMin, m_PSizeMax);
		p->color.r = rangeRand(m_ColorMin.r, m_ColorMax.r);
		p->color.g = rangeRand(m_ColorMin.g, m_ColorMax.g);
		p->color.b = rangeRand(m_ColorMin.b, m_ColorMax.b);
		p->color.a = rangeRand(m_ColorMin.a, m_ColorMax.a);
		m_Count++;
	}
}

bool EpParticleSystem::Render()
{
	if (m_Count == 0)
	{
		return true;
	}
	stParticleVertex *p;
	if (FAILED(m_pVB->Lock(0, sizeof(stParticleVertex) * m_Count, (void**)&p, 0)))
		return false;

	for (int i = 0; i < m_Count; i++)
	{
		p[i].pos = m_Particles[i].pos;
		p[i].psize = m_Particles[i].psize;
		p[i].color = D3DCOLOR_COLORVALUE(m_Particles[i].color.r,
			m_Particles[i].color.g, m_Particles[i].color.b, m_Particles[i].color.a);
	}

	m_pVB->Unlock();

	m_pDevice->SetTransform(D3DTS_WORLD, &m_WorldMat);
	m_pDevice->SetTexture(0, m_pTexture);
	m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	m_pDevice->SetRenderState(D3DRS_POINTSPRITEENABLE, TRUE);
	m_pDevice->SetRenderState(D3DRS_POINTSCALEENABLE, TRUE);
	m_pDevice->SetRenderState(D3DRS_POINTSCALE_A, FtoDW(0.0f));
	m_pDevice->SetRenderState(D3DRS_POINTSCALE_B, FtoDW(0.0f));
	m_pDevice->SetRenderState(D3DRS_POINTSCALE_C, FtoDW(10.0f));

	m_pDevice->SetFVF(D3DFVF_PARTICLE);
	m_pDevice->SetStreamSource(0, m_pVB, 0, sizeof(stParticleVertex));
	m_pDevice->DrawPrimitive(D3DPT_POINTLIST, 0, m_Count);

	m_pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pDevice->SetRenderState(D3DRS_POINTSPRITEENABLE, FALSE);

	return true;
}

EpParticleSystem::~EpParticleSystem()
{
	if (m_Particles != NULL)
	{
		delete[] m_Particles;
		m_Particles = NULL;
	}
	if (m_pVB != NULL)
	{
		m_pVB->Release();
		m_pVB = NULL;
	}
}