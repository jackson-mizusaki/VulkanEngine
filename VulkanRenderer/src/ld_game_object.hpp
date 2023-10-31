#pragma once

#include "ld_model.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <unordered_map>
namespace ld {
	struct TransformComponent {
		glm::vec3 translation{};
		glm::vec3 scale{ 1.f,1.f,1.f };
		glm::vec3 rotation{};

		// Matrix corrsponds to Translate * Ry * Rx * Rz * Scale
		// Rotations correspond to Tait-bryan angles of Y(1), X(2), Z(3)
		// https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
		glm::mat4 mat4();
		glm::mat3 normalMatrix();
	};

	struct PointLightComponent {
		float lightIntensity = 1.0f;
	};

	class LdGameObject {
	public:
		using id_t = unsigned int;
		using Map = std::unordered_map<id_t, LdGameObject>;
		LdGameObject(const LdGameObject&) = delete;
		LdGameObject& operator=(const LdGameObject&) = delete;
		LdGameObject(LdGameObject&&) = default;
		LdGameObject& operator=(LdGameObject&&) = default;

	private:
		LdGameObject(id_t objId) : id{ objId } {}

	public:
		glm::vec3 color{};
		TransformComponent transform{};

		// optional components
		std::shared_ptr<LdModel> model{};
		std::unique_ptr<PointLightComponent> pointLight = nullptr;
	private:
		id_t id;

	public:
		static LdGameObject makePointLight(float intensity = 10.f, float radius = 0.1f, glm::vec3 color = glm::vec3(1.f));
		static LdGameObject createGameObject()
		{
			static id_t currentId = 0;
			return LdGameObject{ currentId++ };
		}
		const id_t getId() { return id; }
	};
}