#pragma once

#include <json.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <vector>
#include <string>

using json = nlohmann::json;

namespace Ld {
	struct TransformComponent {
		enum RotationType {
			TaitBryan,
			Euler,
			Quaternion
		};
		void setTransform(std::vector<float> flatMat);
		glm::vec3 translation{ 0.f, 0.f, 0.f };
		glm::vec3 scale{ 1.f,1.f,1.f };
		glm::vec4 rotation{ 0.f, 0.f, 0.f, 1.f };
		glm::quat quaternion;

		RotationType rotationType = TaitBryan;

		// Matrix corrsponds to Translate * Ry * Rx * Rz * Scale
		// Rotations correspond to Tait-bryan angles of Y(1), X(2), Z(3)
		// https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
		glm::mat4 mat4();
		glm::mat3 normalMatrix();
	};

	class Node {
	public: // constructors
		Node();

	public: // data
		TransformComponent transform{};
		std::vector<Node*> children;
		std::string name;
		json extensions;
		json extras;
	};
}