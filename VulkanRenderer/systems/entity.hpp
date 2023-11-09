#pragma once
#include "stdint.h"

namespace Ld::EntitySystem {
	class Entity {
	public: // constructors
		Entity(uint32_t id);
	public: // functions
		uint32_t getId() const { return m_id; }
	private: // data
		uint32_t m_id;
	};
}