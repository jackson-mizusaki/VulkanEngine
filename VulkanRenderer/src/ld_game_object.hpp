#pragma once

#include "ld_model.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <unordered_map>

// being replaced by Node
namespace Ld {
	struct PointLightComponent {
		float lightIntensity = 1.0f;
	};

	class GameObject {
	public: // types
		using id_t = unsigned int;
		using Map = std::unordered_map<id_t, GameObject>;

	public: // constructors
		GameObject(const GameObject&) = delete;
		GameObject& operator=(const GameObject&) = delete;
		GameObject(GameObject&&) = default;
		GameObject& operator=(GameObject&&) = default;
	private:
		GameObject(id_t objId) : m_id{ objId } {}

	public: // data
		glm::vec3 color{};
		TransformComponent transform{};

		// optional components
		std::shared_ptr<Model> model{};
		std::unique_ptr<PointLightComponent> pointLight = nullptr;
	private:
		id_t m_id;

	public: // functions
		static GameObject makePointLight(float intensity = 10.f, float radius = 0.1f, glm::vec3 color = glm::vec3(1.f));
		static GameObject createGameObject()
		{
			static id_t currentId = 0;
			return GameObject{ currentId++ };
		}
		const id_t getId() { return m_id; }
	};
}