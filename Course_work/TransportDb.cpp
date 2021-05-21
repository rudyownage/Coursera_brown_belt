#include "TransportDb.h"

using namespace std;

namespace TransportDataBase {

    BusManager::BusManager(std::vector<Descriptions::InputQuery> queries, const Json::Dict& routing_settings_json) {
        auto stops_end = partition(queries.begin(), queries.end(), [](const auto& item) {
            return holds_alternative<Descriptions::Stop>(item);
            });

        Descriptions::StopsDict stops_dict;
        for (const auto& item : Range{ begin(queries), stops_end }) {
            const auto& stop = get<Descriptions::Stop>(item);
            stops_dict[stop.name] = &stop;
            stops_.insert({ stop.name, {} });
        }

        Descriptions::BusesDict buses_dict;
        for (const auto& item : Range{ stops_end, end(queries) }) {
            const auto& bus = get<Descriptions::Bus>(item);

            buses_dict[bus.name] = &bus;
            buses_[bus.name] = Bus{
              bus.stops.size(),
              ComputeUniqueItemsCount(AsRange(bus.stops)),
              ComputeRoadRouteLength(bus.stops, stops_dict),
              ComputeGeoRouteDistance(bus.stops, stops_dict)
            };

            for (const string& stop_name : bus.stops) {
                stops_.at(stop_name).bus_names.insert(bus.name);
            }
        }
        router_ = make_unique<TransportRouter>(stops_dict, buses_dict, routing_settings_json);
    }

   const BusManager::Stop* BusManager::GetStop(const string& name) const {
       return GetValuePointer(stops_, name);
   }

   const BusManager::Bus* BusManager::GetBus(const string& name) const {
       return GetValuePointer(buses_, name);
   }

   optional<TransportRouter::RouteInfo> BusManager::FindRoute(const string& stop_from, const string& stop_to) const {
       return router_->FindRoute(stop_from, stop_to);
   }

   int BusManager::ComputeRoadRouteLength(const vector<string>& stops, const Descriptions::StopsDict& stops_dict) {
       int result = 0;
       for (size_t i = 1; i < stops.size(); ++i) {
           result += Descriptions::ComputeStopsDistance(*stops_dict.at(stops[i - 1]), *stops_dict.at(stops[i]));
       }
       return result;
   }

   double BusManager::ComputeGeoRouteDistance(const vector<string>& stops, const Descriptions::StopsDict& stops_dict) {
       double result = 0;
       for (size_t i = 1; i < stops.size(); ++i) {
           result += Sphere::Distance(stops_dict.at(stops[i - 1])->position, stops_dict.at(stops[i])->position);
       }
       return result;
   }

   std::ostream& PrintStop(std::ostream& stream, const Descriptions::Stop& stop) {
       stream << stop.name << " " << stop.position.latitude << " " << stop.position.longitude << endl;
       return stream;
   }

   std::ostream& PrintBus(std::ostream& stream, const Responses::Bus& bus) {
       stream << bus.stop_count << " " << bus.unique_stop_count << " " << bus.geo_route_length << endl;
       return stream;
   }

   void BusManager::ProcessQueries(std::istream& stream) {
       cout << std::setprecision(6);
       const size_t request_count = ReadNumberOnLine<size_t>(stream);
       for (size_t i = 0; i < request_count; ++i) {
           string query, query_type;
           getline(stream, query);
           string_view query_view(query);
           query_type = Descriptions::ReadToken(query_view, " ");
           if (query_type == "Bus") {
               ProcessBus(query_view);
           }
           else if (query_type == "Stop") {
               ProcessStop(query_view);
           }
       }
   }

   void BusManager::ProcessBus(string_view& query_view) const {
       string bus_number = Descriptions::ReadToken(query_view).data();
       if (auto cur_bus = buses_.find(bus_number); cur_bus != buses_.end()) {
           cout << "Bus " << bus_number << ": " << cur_bus->second.stop_count << " stops on route, " << cur_bus->second.unique_stop_count << " unique stops, "
               << cur_bus->second.road_route_length << " route length, " << cur_bus->second.road_route_length / cur_bus->second.geo_route_length << " curvature" <<  endl;
       }
       else {
           cout << "Bus " << bus_number << ": not found" << endl;
       }
   }

   void BusManager::ProcessStop(std::string_view& query_view) const {
       string stop = Descriptions::ReadToken(query_view).data();
       cout << "Stop " << stop << ": ";
       if (stops_.count(stop) == 0) {
           cout << "not found" << endl;
           return;
       }
       if (stops_.at(stop).bus_names.size() == 0) {
           cout << "no buses" << endl;
           return;
       }
       cout << "buses";
       for (auto& bus : stops_.at(stop).bus_names) {
           cout << " " << bus;
       }
       cout << endl;
       
   }
 
}

