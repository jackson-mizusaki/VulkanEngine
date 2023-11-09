#pragma once

#include <json.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <vector>
#include <string>

using json = nlohmann::json;

namespace Ld {


	class Node {
		struct TransformComponent {
			enum RotationType {
				TaitBryan,
				Euler,
				Quaternion
			};
			void setTransform(std::vector<float> flatMat);
			glm::vec3 translation{ 0.f, 0.f, 0.f };
			glm::vec3 scale{ 1.f,1.f,1.f };
			glm::vec3 rotation{ 0.f, 0.f, 0.f};
			glm::quat quaternion{ 0.f, 0.f, 0.f, 1.f };

			RotationType rotationType = TaitBryan;

			// Matrix corrsponds to Translate * Ry * Rx * Rz * Scale
			// Rotations correspond to Tait-bryan angles of Y(1), X(2), Z(3)
			// https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
			glm::mat4 mat4();
			glm::mat3 normalMatrix();
		};
	public: // constructors
		Node();
		virtual ~Node() = default;
		virtual void  addChild(Node* child);
	protected:// functiosn
	public: // data
		TransformComponent transform{};
		std::string name;
		json extensions;
		json extras;
	protected:
		std::vector<Node*> children{};
	};
}