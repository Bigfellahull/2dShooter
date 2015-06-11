#pragma once

#include "Singleton.h"
#include "Texture2d.h"
#include <memory>

namespace SGS2D
{
	class Art : public Singleton<Art>
	{
	public:
		Art();
		Texture2d* GetPlayer();
		Texture2d* GetBullet();
		Texture2d* GetSeeker();
		Texture2d* GetWanderer();
		Texture2d* GetBlackHole();

        Texture2d* GetStarfield();

		Texture2d* GetParticle();
        Texture2d* GetGlowParticle();
        
        Texture2d* Get1x1();
		
        void Load(ID3D11Device& d3dDevice);
	private:
		std::unique_ptr<Texture2d> _player;
		std::unique_ptr<Texture2d> _bullet;
		std::unique_ptr<Texture2d> _seeker;
		std::unique_ptr<Texture2d> _wanderer;
		std::unique_ptr<Texture2d> _blackhole;

        std::unique_ptr<Texture2d> _starfield;

		std::unique_ptr<Texture2d> _particle;
        std::unique_ptr<Texture2d> _glowParticle;

        std::unique_ptr<Texture2d> _1x1;
	};
}