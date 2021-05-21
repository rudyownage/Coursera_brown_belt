#pragma once
#include <array>
#include <cassert>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string_view>
#include <utility>
#include "sphere.h"
#include <variant>
#include <vector>
#include "utils.h"
#include "json.h"

namespace Descriptions {
	struct Stop {
		std::string name;
		Sphere::Point position;
		std::unordered_map<std::string, int> distances;

		static Stop ParseStop(const Json::Dict& attrs);
	};

	struct Bus {
		std::string name;
		std::vector<std::string> stops;

		static Bus ParseBus(const Json::Dict& attrs);
	};

	using InputQuery = std::variant<Stop, Bus>;

	std::ostream& operator<<(std::ostream& stream, const Stop& stop);
	std::ostream& operator<<(std::ostream& stream, const Bus& bus);

	void Print(const std::vector<InputQuery>& queries);

	std::pair<std::string_view, std::optional<std::string_view>> SplitTwoStrict(std::string_view s, std::string_view delimiter = " ");
	std::pair<std::string_view, std::string_view> SplitTwo(std::string_view s, std::string_view delimiter = " ");
	std::string_view ReadToken(std::string_view& s, std::string_view delimiter = " ");
	int ConvertToInt(std::string_view str);
	double ConvertToDouble(std::string_view str);
	int ComputeStopsDistance(const Stop& lhs, const Stop& rhs);

	std::vector<std::string> ParseStops(const std::vector<Json::Node>& stop_nodes, bool is_roundtrip);
	template <typename Object>
	using Dict = std::unordered_map<std::string, const Object*>;

	using StopsDict = Dict<Stop>;
	using BusesDict = Dict<Bus>;


	std::vector<Descriptions::InputQuery> ReadDescriptions(const std::vector<Json::Node>& nodes);
}