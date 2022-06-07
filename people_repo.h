#pragma once

#include "person.h"

#include <map>

struct people_repo
{
    std::map<std::string, person> people;

    void new_person(person p)
    {
        people.emplace(p.name, std::move(p));
    }

    person* find_person(const std::string& name)
    {
        auto it = people.find(name);
        if (it == people.end()) return nullptr;

        return &it->second;
    }
};
