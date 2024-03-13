#pragma once

#include <any>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <typeinfo>
#include <unordered_map>

#include "core/event/event_types.hpp"


class Event
{
public:
    Event() = delete;

    explicit Event(EventId id)
        : mId(id)
    {}

    template<typename T>
    Event& SetParam(ParamId id, T value)
    {
        mData[id] = {value, typeid(value).hash_code()};
        return *this;
    }

    template<typename T>
    T GetParam(ParamId id) const
    {
        try {
            EventData param = mData.at(id);
            if (typeid(T).hash_code() == param.typeHash)
            {
                return std::any_cast<T>(param.data);
            }
            std::cerr << "[ERROR]\t"
                      << "Event param type is invalid: " << typeid(T).name()
                      << std::endl;
            exit(EXIT_FAILURE);
        } catch (std::out_of_range const& e) {
            std::cout << "[ERROR]\t"
                      << "Event param id does not exist: " << id << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    EventId GetId() const
    {
        return mId;
    }

private:
    struct EventData
    {
        std::any data;
        size_t typeHash;
    };

    EventId mId { };
    std::unordered_map<ParamId, EventData> mData{};
};

