#pragma once 

#include "scene_node.hpp"
#include <json.hpp>


#include <vector>
#include <string>

using json = nlohmann::json;

namespace Ld {
	enum TargetPath {
		Translation,
		Rotation,
		Scale,
		Weights
	};

	enum InterpolationType {
		Linear,
		Step,
		CubicSpline
	};
	class Animation {
	public: // Types
		class Channel {
		public: // types
			class Target {
			public: // data
				SceneNode targetNode;
				TargetPath path;
				//json extensions;
				//json extras;
			};
		};

		class Sampler {
		public: // data
			uint32_t input; // accessor Index containing keyframe timestamps. 
			InterpolationType interpolation;
			uint32_t output; // accessor index containing keyframe output values
			//json extensions;
			//json extras;
		};

	public: // functions


	public: // data
		std::vector<Channel> channels;
		std::vector<Sampler> samplers;

	};
}