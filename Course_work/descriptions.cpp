#include "descriptions.h"

using namespace std;

namespace Descriptions {
    pair<string_view, optional<string_view>> SplitTwoStrict(string_view s, string_view delimiter) {
        const size_t pos = s.find(delimiter);
        if (pos == s.npos) {
            return { s, nullopt };
        }
        else {
            return { s.substr(0, pos), s.substr(pos + delimiter.length()) };
        }
    }

    pair<string_view, string_view> SplitTwo(string_view s, string_view delimiter) {
        const auto [lhs, rhs_opt] = SplitTwoStrict(s, delimiter);
        return { lhs, rhs_opt.value_or("") };
    }

    string_view ReadToken(string_view& s, string_view delimiter) {
        const auto [lhs, rhs] = SplitTwo(s, delimiter);
        s = rhs;
        return lhs;
    }

    int ConvertToInt(string_view str) {
        // use std::from_chars when available to git rid of string copy
        size_t pos;
        const int result = stoi(string(str), &pos);
        if (pos != str.length()) {
            std::stringstream error;
            error << "string " << str << " contains " << (str.length() - pos) << " trailing chars";
            throw invalid_argument(error.str());
        }
        return result;
    }

    double ConvertToDouble(string_view str) {
        // use std::from_chars when available to git rid of string copy
        size_t pos;
        const double result = stod(string(str), &pos);
        if (pos != str.length()) {
            std::stringstream error;
            error << "string " << str << " contains " << (str.length() - pos) << " trailing chars";
            throw invalid_argument(error.str());
        }
        return result;
    }

    Stop Stop::ParseStop(const Json::Dict& attrs) {
        Stop stop = {
            .name = attrs.at("name").AsString(),
            .position = {
                .latitude = attrs.at("latitude").AsDouble(),
                .longitude = attrs.at("longitude").AsDouble()
            }
        };
        if (attrs.count("road_distances") > 0) {
            for (const auto& [neighbour_stop, distance_node] : attrs.at("road_distances").AsMap()) {
                stop.distances[neighbour_stop] = distance_node.AsInt();
            }
        }
        return stop;
    }

    vector<string> ParseStops(const std::vector<Json::Node>& stop_nodes, bool is_roundtrip) {
        vector<string> stops;
        stops.reserve(stop_nodes.size());
        for (const Json::Node& stop_node : stop_nodes) {
            stops.push_back(stop_node.AsString());
        }
        if (is_roundtrip || stops.size() <= 1) {
            return stops;
        }
        stops.reserve(stops.size() * 2 - 1);
        for (size_t stop_idx = stops.size() - 1; stop_idx > 0; --stop_idx) {
            stops.push_back(stops[stop_idx - 1]);
        }
        return stops;
    }

    Bus Bus::ParseBus(const Json::Dict& attrs) {
        return Bus{
            .name = attrs.at("name").AsString(),
            .stops = ParseStops(attrs.at("stops").AsArray(), attrs.at("is_roundtrip").AsBool()),
        };
    }

    ostream& operator<<(ostream& stream, const Stop& stop) {
        stream << stop.name << ": " << stop.position.latitude << " " << stop.position.longitude << endl;
        return stream;
    }

    ostream& operator<<(ostream& stream, const Bus& bus) {
        stream << bus.name << ": ";
        for (auto& item : bus.stops) {
            stream << item << ", ";
        }
        stream << endl;
        return stream;
    }

    int ComputeStopsDistance(const Stop& lhs, const Stop& rhs) {
        if (auto it = lhs.distances.find(rhs.name); it != lhs.distances.end()) {
            return it->second;
        }
        else {
            return rhs.distances.at(lhs.name);
        }
    }


    std::vector<Descriptions::InputQuery> ReadDescriptions(const vector<Json::Node>& nodes) {

        vector<InputQuery> result;
        result.reserve(nodes.size());

        for (const Json::Node& node : nodes) {
            const auto& node_dict = node.AsMap();
            if (node_dict.at("type").AsString() == "Bus") {
                result.push_back(Bus::ParseBus(node_dict));
            }
            else {
                result.push_back(Stop::ParseStop(node_dict));
            }
        }

        return result;
    }

}