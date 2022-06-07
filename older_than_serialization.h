#pragma once

#include "older_than.h"
#include "people_repo.h"

#include <boost/serialization/map.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/version.hpp>

#include <stdexcept>

struct older_than_load_helper
{
    people_repo* repo = nullptr;

    struct null_repo : std::exception
    {
        constexpr const char* what() const noexcept
        {
            return "People repo was not initialized before load";
        }
    };
};

namespace boost::serialization {
    template <typename Archive>
    void serialize(Archive& ar, older_than::cache_key& key, const unsigned version)
    { split_free(ar, key, version); }

    template <typename Archive>
    void save(Archive& ar, const older_than::cache_key& key, const unsigned version)
    {
        ar << key.first->name;
        ar << key.second->name;
    }

    template <typename Archive>
    void load(Archive& ar, older_than::cache_key& key, const unsigned version)
    {
        std::string firstName, secondName;
        ar >> firstName;
        ar >> secondName;

        auto& h = ar.template get_helper<older_than_load_helper>();
        if (!h.repo)
            throw older_than_load_helper::null_repo{};

        key.first = h.repo->find_person(std::move(firstName));
        key.second = h.repo->find_person(std::move(secondName));
    }

    template <typename Archive>
    void serialize(Archive& ar, older_than& ot, const unsigned version)
    {
        ar & ot.cache;
    }
}

BOOST_CLASS_VERSION(older_than::cache_key, 0)
BOOST_CLASS_VERSION(older_than, 0)
