#pragma once

#include "entity.hpp"
#include "component.hpp"

#include <vector>
#include <map>
#include <typeinfo>
#include <memory>
namespace Ld::EntitySystem {
	class EntityManager {
	public: // types
		enum class ComponentType {

		};
		using Component_ptr = std::shared_ptr<Component>;
		using ComponentPtrMap = std::map<uint32_t, Component_ptr>;
	public: // constructors
		EntityManager();
	public: // functions
		static uint32_t getId()
		{
			static uint32_t id = m_nextId++;
			return id;
		}
		Entity* createEntity();
		void addComponent(uint32_t entityId, const Component_ptr& component);
		Component_ptr getComponentOfNode(uint32_t entityId, std::string componentName);
		std::vector<Entity*> getAllEntitiesWithNamedComponents(std::string componentName);
	public: // data
	private:
		std::map<std::string, ComponentPtrMap> m_entityComponentMap;
		std::vector<Entity*> m_entities;
		static uint32_t m_nextId;
	};
}