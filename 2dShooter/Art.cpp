#include "stdafx.h"
#include "Art.h"
#include "DDSTextureLoader.h"

using namespace SGS2D;
using namespace DirectX;

Art::Art()
{
	_player.reset(new Texture2d());
	_bullet.reset(new Texture2d());
	_seeker.reset(new Texture2d());
	_wanderer.reset(new Texture2d());
	_blackhole.reset(new Texture2d());
    _starfield.reset(new Texture2d());
	
	_particle.reset(new Texture2d());
    _glowParticle.reset(new Texture2d());

    _1x1.reset(new Texture2d());
}

void Art::Load(ID3D11Device& d3dDevice)
{
    _player->Load(&d3dDevice, L"..\\Debug\\Player.dds");
    _bullet->Load(&d3dDevice, L"..\\Debug\\Bullet.dds");
    _seeker->Load(&d3dDevice, L"..\\Debug\\Seeker.dds");
    _wanderer->Load(&d3dDevice, L"..\\Debug\\Wanderer.dds");
    _blackhole->Load(&d3dDevice, L"..\\Debug\\BlackHole.dds");
    _starfield->Load(&d3dDevice, L"..\\Debug\\Starfield.dds");

    _particle->Load(&d3dDevice, L"..\\Debug\\Laser.dds");
    _glowParticle->Load(&d3dDevice, L"..\\Debug\\Glow.dds");

    _1x1->Load(&d3dDevice, L"..\\Debug\\1x1.dds");
}

Texture2d* Art::GetPlayer()
{
	return _player.get();
}

Texture2d* Art::GetBullet()
{
	return _bullet.get();
}

Texture2d* Art::GetSeeker()
{
	return _seeker.get();
}

Texture2d* Art::GetWanderer()
{
	return _wanderer.get();
}

Texture2d* Art::GetBlackHole()
{
	return _blackhole.get();
}

Texture2d* Art::GetStarfield()
{
    return _starfield.get();
}

Texture2d* Art::GetParticle()
{
	return _particle.get();
}

Texture2d* Art::GetGlowParticle()
{
    return _glowParticle.get();
}

Texture2d* Art::Get1x1()
{
    return _1x1.get();
}