#pragma once

#include "json.h"
#include "TransportDb.h"

#include <string>
#include <variant>


namespace Requests {
    struct Stop {
        std::string name;

        Json::Dict Process(const TransportDataBase::BusManager& db) const;
    };

    struct Bus {
        std::string name;

        Json::Dict Process(const TransportDataBase::BusManager& db) const;
    };

    struct Route {
        std::string stop_from;
        std::string stop_to;

        Json::Dict Process(const TransportDataBase::BusManager& db) const;
    };

    std::variant<Stop, Bus, Route> Read(const Json::Dict& attrs);

    std::vector<Json::Node> ProcessAll(const TransportDataBase::BusManager& db, const std::vector<Json::Node>& requests);
}
