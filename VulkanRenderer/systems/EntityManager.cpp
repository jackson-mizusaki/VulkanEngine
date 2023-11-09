#include "EntityManager.hpp"

namespace Ld::EntitySystem
{
	EntityManager::EntityManager()
	{

	}

	Entity* EntityManager::createEntity()
	{
		uint32_t id = EntityManager::getId();
		Entity* newEntity = new Entity(id);
		m_entities.push_back(newEntity);
		return newEntity;
	}
	void EntityManager::addComponent(uint32_t entityId, const Component_ptr& component)
	{
		try
		{
			ComponentPtrMap& componentMap = m_entityComponentMap.at(typeid(*component).name());

			componentMap.emplace(entityId, component);

		}
		catch (std::exception e)
		{
			throw e;
		}
	}

	EntityManager::Component_ptr EntityManager::getComponentOfNode(uint32_t entityId, std::string componentName)
	{
		try
		{
			ComponentPtrMap componentMap = m_entityComponentMap.at(componentName);
			Component_ptr component = componentMap.at(entityId);
			return component;
		}
		catch (std::exception e)
		{
			throw e;
		}
	}
	std::vector<Entity*> EntityManager::getAllEntitiesWithNamedComponents(std::string componentName)
	{
		try
		{
			ComponentPtrMap componentMap = m_entityComponentMap.at(componentName);
			std::vector<Entity*> returnVector;
			for (auto [key, val] : componentMap)
			{
				returnVector.push_back(m_entities[key]);
			}
			return returnVector;
		}
		catch (std::exception e)
		{
			throw e;
		}

	}
}
