#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <optional>
#include "nlohmann/json.hpp"
#include "SceneData.h"

namespace nl = nlohmann;

namespace nlohmann {
	template <typename T>
	struct adl_serializer<std::optional<T>> {
		static void to_json(json& j, const std::optional<T>& opt) {
			if (opt == std::nullopt) {
				j = nullptr;
			}
			else {
				j = *opt; // this will call adl_serializer<T>::to_json which will
						  // find the free function to_json in T's namespace!
			}
		}

		static void from_json(const json& j, std::optional<T>& opt) {
			if (j.is_null()) {
				opt = std::nullopt;
			}
			else {
				opt = j.get<T>(); // same as above, but with
								  // adl_serializer<T>::from_json
			}
		}
	};
}

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(SceneData, isMainLight, MainLightColor, MainLightPosition, MainLightEulerangle, MainLightForward, MainLightName, RenderObjectName, mMaterialType, MaterialPath, MeshPath, RenderObjectPosition, RenderObjectEulerAngle, RenderObjectScale)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MaterialLitData, Name, MatCBIndex, NumFramesDirty, DiffuseColor, Smoothness, Metallic, Occlusion, EmissionStrength, EmissionColor, UseNormalMap, NormalScale, DiffuseMapIndex, NormalMapIndex, MsoMapIndex, EmissionMapIndex, LUTMapIndex)