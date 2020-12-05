#include "OnlineManager.h"

std::unordered_map<std::string, void*> OnlineManager::m_InstancePtrMap;

OnlineManager::OnlineManager()
{
}

OnlineManager::~OnlineManager()
{
}

void OnlineManager::Update()
{
}

void OnlineManager::DestoryAllManagers()
{
	for (const auto manager : m_InstancePtrMap)
	{
		free(manager.second);
	}
	m_InstancePtrMap.clear();
}