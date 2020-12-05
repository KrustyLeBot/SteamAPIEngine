#pragma once
#include <cstdlib>
#include<iostream>
#include<vector>
#include <unordered_map>
#include <typeindex>
#include "Classes/CpuTimer.h"

class OnlineManager
{
public:
	OnlineManager();
	~OnlineManager();

	virtual void Update();

	template<class T>
	static T* GetInstancePtr()
	{
		std::string name = typeid(T).name();
		if (m_InstancePtrMap[name] == nullptr)
		{
			void* mem = std::malloc(sizeof(T));
			m_InstancePtrMap[name] = new (mem) T();
		}
		return static_cast<T*>(m_InstancePtrMap[name]);
	}

	static void DestoryAllManagers();

	static std::unordered_map<std::string, void*> m_InstancePtrMap;

protected:
	CpuTimer m_timer;
};