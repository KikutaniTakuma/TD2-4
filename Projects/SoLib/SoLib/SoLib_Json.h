#pragma once
#include "SoLib_Traits.h"
#include <variant>

#include "../../externals/nlohmann/json.hpp"

#include "Math/Mat4x4.h"
#include "Math/Quaternion.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include <string>

namespace SoLib {

	template<::SoLib::IsNotPointer T>
	using JsonItemPtr = std::variant<bool T:: *, int32_t T:: *, float T:: *, std::string T:: *, Vector2 T:: *, Vector3 T:: *, Vector4 T:: *, Quaternion T:: *>;

	template<::SoLib::IsNotPointer T>
	using JsonPair = std::pair<std::string, std::list<JsonItemPtr<T>>>;

	template<::SoLib::IsNotPointer T>
	using JsonPairList = std::list<std::variant<JsonItemPtr<T>, JsonPair<T>>>;

	template<::SoLib::IsNotPointer T>
	inline JsonPairList<T> GetJsonPairList() { return JsonPairList<T>{}; }
	/*template<>
		inline JsonPairList<Quaternion> GetJsonPairList<Quaternion>() {
		return {
			JsonPair<Quaternion>{ "vec", { reinterpret_cast<Vector3 Quaternion:: *>(&Quaternion::m)}},
			JsonPair<Quaternion>{ "w", { reinterpret_cast<float Quaternion:: *>(& Quaternion::quaternion::w) }}
		};
	}*/

	template<::SoLib::IsNotPointer T>
	inline void ToJson(nlohmann::json &json, const T &data);

	template<::SoLib::IsNotPointer T>
	inline void FromJson(const nlohmann::json &json, T &data);
}

void from_json(const nlohmann::json &json, Vector2 &data);
void from_json(const nlohmann::json &json, Vector3 &data);
void from_json(const nlohmann::json &json, Vector4 &data);

void to_json(nlohmann::json &json, const Quaternion &data);
void from_json(const nlohmann::json &json, Quaternion &data);

#pragma region Func

namespace SoLib {

	template<::SoLib::IsNotPointer T>
	inline void ToJson(nlohmann::json &json, const T &data) {
		json; data;
		/*static const auto &jsonPair = GetJsonPairList<T>();


		for (const auto &jsonData : jsonPair) {
			if (std::holds_alternative<JsonItemPtr<T>>(jsonData)) {
				if (jsonPair.size() == 1u) {
					std::visit([&json, &data](auto T:: *const ptrv)
						{
							json = data.*ptrv;
						}, std::get<JsonItemPtr<T>>(jsonData));
				}
				else {
					std::visit([&json, &data](auto T:: *const ptrv)
						{
							json.push_back(data.*ptrv);
						}, std::get<JsonItemPtr<T>>(jsonData));
				}
			}
			else {
				auto &key = std::get<JsonPair<T>>(jsonData).first;
				auto &ptrList = std::get<JsonPair<T>>(jsonData).second;

				if (ptrList.size() == 1u) {

					auto &jsonItem = json[key];
					std::visit([&jsonItem, &data](auto T:: *const ptrv)
						{
							jsonItem = data.*ptrv;
						}, ptrList.front());
				}
				else {
					for (const auto &member : ptrList) {

						auto &jsonItem = json[key];
						std::visit([&jsonItem, &data](auto T:: *const ptrv)
							{
								jsonItem.push_back(data.*ptrv);
							}, member);

					}
				}
			}

		}*/
	}

	template<::SoLib::IsNotPointer T>
	inline void FromJson(const nlohmann::json &json, T &data) {
		json; data;
		/*static const auto &jsonPair = GetJsonPairList<T>();

		uint32_t jsonDataItr = 0u;
		for (const auto &jsonData : jsonPair) {
			if (std::holds_alternative<JsonItemPtr<T>>(jsonData)) {
				if (jsonPair.size() == 1u) {
					std::visit([&json, &data](auto T:: *const ptrv)
						{
							data.*ptrv = json;
						}, std::get<JsonItemPtr<T>>(jsonData));
				}
				else {
					std::visit([&json, &data, jsonDataItr](auto T:: *const ptrv)
						{
							data.*ptrv = json.at(jsonDataItr);
						}, std::get<JsonItemPtr<T>>(jsonData));
				}
			}
			else {
				auto &key = std::get<JsonPair<T>>(jsonData).first;
				auto &ptrList = std::get<JsonPair<T>>(jsonData).second;

				if (ptrList.size() == 1u) {

					const auto &jsonItem = json.at(key);
					std::visit([&jsonItem, &data](auto T:: *const ptrv)
						{
							data.*ptrv = jsonItem;
						}, ptrList.front());

				}
				else {
					uint32_t index = 0u;
					for (const auto &member : ptrList) {
						const auto &jsonItem = json.at(key).at(index);
						std::visit([&jsonItem, &data](auto T:: *const ptrv)
							{
								data.*ptrv = jsonItem;
							}, member);
						index++;
					}
				}
			}
			jsonDataItr++;

		}*/
	}

}

#pragma endregion