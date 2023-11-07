#include "node.hpp"

namespace Ld {
	void Node::TransformComponent::setTransform(std::vector<float> flatMat)
	{
		// TODO finish this
		assert(flatMat.size() == 16);
		translation = glm::vec3(flatMat[12], flatMat[13], flatMat[14]);
		scale.x = glm::length(glm::vec3(flatMat[0], flatMat[1], flatMat[2])); // Basis vector X
		scale.y = glm::length(glm::vec3(flatMat[4], flatMat[5], flatMat[6])); // Basis vector Y
		scale.z = glm::length(glm::vec3(flatMat[8], flatMat[9], flatMat[10])); // Basis vector 

		// rotation is difficult. pull it out into a quaternion?
		switch (rotationType)
		{
		case TaitBryan:
			break;
		}
	}

	glm::mat4 Node::TransformComponent::mat4()
	{

		const float c3 = glm::cos(rotation.z);
		const float s3 = glm::sin(rotation.z);
		const float c2 = glm::cos(rotation.x);
		const float s2 = glm::sin(rotation.x);
		const float c1 = glm::cos(rotation.y);
		const float s1 = glm::sin(rotation.y);
		return glm::mat4{
			{
				scale.x * (c1 * c3 + s1 * s2 * s3),
				scale.x * (c2 * s3),
				scale.x * (c1 * s2 * s3 - c3 * s1),
				0.0f,
			},
			{
				scale.y * (c3 * s1 * s2 - c1 * s3),
				scale.y * (c2 * c3),
				scale.y * (c1 * c3 * s2 + s1 * s3),
				0.0f,
			},
			{
				scale.z * (c2 * s1),
				scale.z * (-s2),
				scale.z * (c1 * c2),
				0.0f,
			},
			{translation.x, translation.y, translation.z, 1.0f}
		};
	}

	glm::mat3 Node::TransformComponent::normalMatrix()
	{
		const float c3 = glm::cos(rotation.z);
		const float s3 = glm::sin(rotation.z);
		const float c2 = glm::cos(rotation.x);
		const float s2 = glm::sin(rotation.x);
		const float c1 = glm::cos(rotation.y);
		const float s1 = glm::sin(rotation.y);
		const glm::vec3 invScale = 1.0f / scale;
		return glm::mat3{
			{
				invScale.x * (c1 * c3 + s1 * s2 * s3),
				invScale.x * (c2 * s3),
				invScale.x * (c1 * s2 * s3 - c3 * s1)
			},
			{
				invScale.y * (c3 * s1 * s2 - c1 * s3),
				invScale.y * (c2 * c3),
				invScale.y * (c1 * c3 * s2 + s1 * s3)
			},
			{
				invScale.z * (c2 * s1),
				invScale.z * (-s2),
				invScale.z * (c1 * c2)
			}
		};
	}
	
	Node::Node()
	{
	}
}