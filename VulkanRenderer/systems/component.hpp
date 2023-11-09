#pragma once

#include <string>
namespace Ld::EntitySystem {
	// Abstract base class for components
	class Component {
	public:
		Component();
		virtual ~Component() = default;
	public:
		std::string getName() const { return m_name; }
	protected:
		std::string m_name;
	};
}