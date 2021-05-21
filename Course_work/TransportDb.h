#pragma once
#include <set>
#include <string>
#include <unordered_map>
#include "descriptions.h"
#include "utils.h"
#include <iomanip>
#include <algorithm>
#include "json.h"
#include "transport_router.h"

namespace Responses {
    struct Stop {
        std::set<std::string> bus_names;
    };

    struct Bus {
        size_t stop_count = 0;
        size_t unique_stop_count = 0;
        int road_route_length = 0;
        double geo_route_length = 0.0;
    };
}

namespace TransportDataBase {

	class BusManager {
        using Bus = Responses::Bus;
        using Stop = Responses::Stop;
        std::unordered_map<std::string, Bus> buses_;
        std::unordered_map<std::string, Stop> stops_;
        std::unique_ptr<TransportRouter> router_;

        int ComputeRoadRouteLength(const std::vector<std::string>& stops, const Descriptions::StopsDict& stops_dict);
        double ComputeGeoRouteDistance(const std::vector<std::string>& stops, const Descriptions::StopsDict& stops_dict);

	public:
        BusManager(std::vector<Descriptions::InputQuery> queries, const Json::Dict& routing_settings_json);

        const Stop* GetStop(const std::string& name) const;
        const Bus* GetBus(const std::string& name) const;

        std::optional<TransportRouter::RouteInfo> FindRoute(const std::string& stop_from, const std::string& stop_to) const;

        std::string RenderMap() const;

        void ProcessQueries(std::istream& stream = std::cin);
        void ProcessBus(std::string_view& query_view) const;
        void ProcessStop(std::string_view& query_view) const;

	};

    std::ostream& PrintStop(std::ostream& stream, const Descriptions::Stop&);
    std::ostream& PrintBus(std::ostream& stream, const Responses::Bus&);
}
