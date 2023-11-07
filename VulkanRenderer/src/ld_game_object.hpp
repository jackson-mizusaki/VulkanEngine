#pragma once

#include "ld_model.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <memory>
#include <unordered_map>

// being replaced by Node
namespace Ld {
	struct PointLightComponent {
		float lightIntensity = 1.0f;
	};

	class GameObject {
		struct TransformComponent {
			glm::vec3 translation{};
			glm::vec3 scale{ 1.f, 1.f, 1.f };
			glm::vec3 rotation{};

			// Matrix corrsponds to Translate * Ry * Rx * Rz * Scale
			// Rotations correspond to Tait-bryan angles of Y(1), X(2), Z(3)
			// https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
			glm::mat4 mat4();

			glm::mat3 normalMatrix();
		};

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