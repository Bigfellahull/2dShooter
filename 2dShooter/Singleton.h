#pragma once

namespace SGS2D
{
	template<typename T>
	class Singleton
	{
	public:
		Singleton() { }

		static T* GetInstance()
		{
			static T _instance;
			return &_instance;
		}
	};
}