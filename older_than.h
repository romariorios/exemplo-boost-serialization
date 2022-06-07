#pragma once

#include "person.h"

#include <map>
#include <utility>

#include <iostream>

struct older_than
{
    using cache_key = std::pair<const person*, const person*>;
    std::map<cache_key, bool> cache;

    bool operator()(const person& p1, const person& p2)
    {
        cache_key key{&p1, &p2};
        auto it = cache.find(key);
        if (it != cache.end())
            return it->second;

        std::cout << "Performing very expensive calculation...\n";
        bool result = p1.age > p2.age; // very expensive calculation
        cache.emplace(key, result);

        return result;
    }
};
