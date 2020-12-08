#include "Singleton.h"

std::unordered_map<std::string, void*> Singleton::m_InstancePtrMap;

Singleton::Singleton()
{
}

Singleton::~Singleton()
{
}

void Singleton::Update()
{
}

void Singleton::DestoryAllManagers()
{
	for (const auto manager : m_InstancePtrMap)
	{
		free(manager.second);
	}
	m_InstancePtrMap.clear();
}