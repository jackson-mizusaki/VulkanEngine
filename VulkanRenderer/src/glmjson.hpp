#pragma once

// using solution authored by graymadness
// https://github.com/graymadness/nlohmann_json_glm

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include <json.hpp>

namespace nlohmann
{
	// vec2 
	template<typename T>
	struct adl_serializer<glm::vec<2, T, glm::defaultp>>
	{
		typedef glm::vec<2, T, glm::defaultp> vec;

		static_assert(
			std::is_same<T, float>::value ||
			std::is_same<T, double>::value ||
			std::is_same<T, int8_t>::value ||
			std::is_same<T, int16_t>::value ||
			std::is_same<T, int32_t>::value ||
			std::is_same<T, int64_t>::value ||
			std::is_same<T, uint8_t>::value ||
			std::is_same<T, uint16_t>::value ||
			std::is_same<T, uint32_t>::value ||
			std::is_same<T, uint64_t>::value,
			"Invalid type for vector"
			);

		static void to_json(json& j, const vec& value)
		{
			j = json::array({ value.x, value.y });
		}

		static void from_json(const json& j, vec& value)
		{
			switch (j.type())
			{
			default:
				throw std::runtime_error("Invalid value type");
			case json::value_t::number_float:
			case json::value_t::number_integer:
			case json::value_t::number_unsigned:
			{
				T v = j;
				value = vec(v, v);
				return;
			}
			case json::value_t::array:
			{
				T x = j.size() >= 1 && !j[0].empty() ? j[0].get<T>() : T();
				T y = j.size() >= 2 && !j[1].empty() ? j[1].get<T>() : T();

				value = vec(x, y);
				return;
			}
			case json::value_t::object:
			{
				if (j.empty())
				{
					value = vec(0, 0);
				}
				else
				{
					T x = j.contains("x") && !j["x"].empty() ? j["x"].get<T>() : T();
					T y = j.contains("y") && !j["y"].empty() ? j["y"].get<T>() : T();

					value = vec(x, y);
				}
				return;
			}
			}
		}
	};

	// vec3
	template<typename T>
	struct adl_serializer<glm::vec<3, T, glm::defaultp>>
	{
		typedef glm::vec<3, T, glm::defaultp> vec;

		static_assert(
			std::is_same<T, float>::value ||
			std::is_same<T, double>::value ||
			std::is_same<T, int8_t>::value ||
			std::is_same<T, int16_t>::value ||
			std::is_same<T, int32_t>::value ||
			std::is_same<T, int64_t>::value ||
			std::is_same<T, uint8_t>::value ||
			std::is_same<T, uint16_t>::value ||
			std::is_same<T, uint32_t>::value ||
			std::is_same<T, uint64_t>::value,
			"Invalid type for vector"
			);

		static void to_json(json& j, const vec& value)
		{
			j = json::array({ value.x, value.y, value.z });
		}

		static void from_json(const json& j, vec& value)
		{
			switch (j.type())
			{
			default:
				throw std::runtime_error("Invalid value type");
			case json::value_t::number_float:
			case json::value_t::number_integer:
			case json::value_t::number_unsigned:
			{
				T v = j;
				value = vec(v, v, v);
				return;
			}
			case json::value_t::array:
			{
				T x = j.size() > 0 && !j[0].empty() ? j[0].get<T>() : T();
				T y = j.size() > 1 && !j[1].empty() ? j[1].get<T>() : T();
				T z = j.size() > 2 && !j[2].empty() ? j[2].get<T>() : T();

				value = vec(x, y, z);
				return;
			}
			case json::value_t::object:
			{
				if (j.empty())
				{
					value = vec(0, 0, 0);
				}
				else
				{
					T x = j.contains("x") && !j["x"].empty() ? j["x"].get<T>() : T();
					T y = j.contains("y") && !j["y"].empty() ? j["y"].get<T>() : T();
					T z = j.contains("z") && !j["z"].empty() ? j["z"].get<T>() : T();

					value = vec(x, y, z);
				}
				return;
			}
			}
		}
	};

	// vec4  
	template<typename T>
	struct adl_serializer<glm::vec<4, T, glm::defaultp>>
	{
		typedef glm::vec<4, T, glm::defaultp> vec;

		static_assert(
			std::is_same<T, float>::value ||
			std::is_same<T, double>::value ||
			std::is_same<T, int8_t>::value ||
			std::is_same<T, int16_t>::value ||
			std::is_same<T, int32_t>::value ||
			std::is_same<T, int64_t>::value ||
			std::is_same<T, uint8_t>::value ||
			std::is_same<T, uint16_t>::value ||
			std::is_same<T, uint32_t>::value ||
			std::is_same<T, uint64_t>::value,
			"Invalid type for vector"
			);

		static void to_json(json& j, const vec& value)
		{
			j = json::array({ value.x, value.y, value.z, value.w });
		}

		static void from_json(const json& j, vec& value)
		{
			switch (j.type())
			{
			default:
				throw std::runtime_error("Invalid value type");
			case json::value_t::number_float:
			case json::value_t::number_integer:
			case json::value_t::number_unsigned:
			{
				T v = j;
				value = vec(v, v, v, v);
				return;
			}
			case json::value_t::array:
			{
				T x = j.size() > 0 && !j[0].empty() ? j[0].get<T>() : T();
				T y = j.size() > 1 && !j[1].empty() ? j[1].get<T>() : T();
				T z = j.size() > 2 && !j[2].empty() ? j[2].get<T>() : T();
				T w = j.size() > 3 && !j[3].empty() ? j[3].get<T>() : T();

				value = vec(x, y, z, w);
				return;
			}
			case json::value_t::object:
			{
				if (j.empty())
				{
					value = vec(0, 0, 0, 0);
				}
				else
				{
					T x = j.contains("x") && !j["x"].empty() ? j["x"].get<T>() : T();
					T y = j.contains("y") && !j["y"].empty() ? j["y"].get<T>() : T();
					T z = j.contains("z") && !j["z"].empty() ? j["z"].get<T>() : T();
					T w = j.contains("w") && !j["w"].empty() ? j["w"].get<T>() : T();

					value = vec(x, y, z, w);
				}
				return;
			}
			}
		}
	};

	// mat4
	template<typename T>
	struct adl_serializer<glm::mat<4, 4, T, glm::defaultp>>
	{
		typedef glm::mat<4, 4, T, glm::defaultp> mat;

		static_assert(
			std::is_same<T, float>::value ||
			std::is_same<T, double>::value,
			"Invalid type for matrix"
			);

		static void to_json(json& j, const mat& value)
		{
			j = json::array({
					value[0][0], value[0][1], value[0][2], value[0][3],
					value[1][0], value[1][1], value[1][2], value[1][3],
					value[2][0], value[2][1], value[2][2], value[2][3],
					value[3][0], value[3][1], value[3][2], value[3][3]
				});
		}

		static void from_json(const json& j, mat& value)
		{
			switch (j.type())
			{
			default:
				throw std::runtime_error("Invalid value type");
			case json::value_t::number_float:
			case json::value_t::number_integer:
			case json::value_t::number_unsigned:
			{
				T v = j;
				value = mat(
					v, v, v, v,
					v, v, v, v,
					v, v, v, v,
					v, v, v, v
				);
				return;
			}
			case json::value_t::array:
			{
				std::array<T, 16> arr = {};
				for (std::size_t i = 0; i < arr.size(); i++)
					arr[i] = j.size() > i && !j[i].empty() ? j[i].get<T>() : T();

				value = mat(
					arr[0], arr[1], arr[2], arr[3],
					arr[4], arr[5], arr[6], arr[7],
					arr[8], arr[9], arr[10], arr[11],
					arr[12], arr[13], arr[14], arr[15]
				);
				return;
			}
			}
		}
	};

	// quaternion
	template<typename T>
	struct adl_serializer <glm::tquat<T, glm::defaultp>>
	{
		typedef glm::tquat<T, glm::defaultp> quat;

		static_assert(
			std::is_same<T, float>::value ||
			std::is_same<T, double>::value ||
			"Invalid type for quaternion"
			);

		static void to_json(json& j, const quat& value)
		{
			j = json::array({ value.x, value.y, value.z, value.w });
		}

		static void from_json(const json& j, quat& value)
		{
			switch (j.type())
			{
			default:
				throw std::runtime_error("Invalid value type");
			case json::value_t::number_float:
			{
				T v = j;
				value = quat(v, v, v, v);
				return;
			}
			case json::value_t::array:
			{
				T x = j.size() > 0 && !j[0].empty() ? j[0].get<T>() : T();
				T y = j.size() > 1 && !j[1].empty() ? j[1].get<T>() : T();
				T z = j.size() > 2 && !j[2].empty() ? j[2].get<T>() : T();
				T w = j.size() > 3 && !j[3].empty() ? j[3].get<T>() : T();

				value = quat(x, y, z, w);
				return;
			}
			case json::value_t::object:
			{
				if (j.empty())
				{
					value = quat(0, 0, 0, 1);
				}
				else
				{
					T x = j.contains("x") && !j["x"].empty() ? j["x"].get<T>() : T();
					T y = j.contains("y") && !j["y"].empty() ? j["y"].get<T>() : T();
					T z = j.contains("z") && !j["z"].empty() ? j["z"].get<T>() : T();
					T w = j.contains("w") && !j["w"].empty() ? j["w"].get<T>() : T();

					value = quat(x, y, z, w);
				}
				return;
			}
			}
		}
	};

}