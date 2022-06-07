#include "older_than.h"
#include "older_than_serialization.h"
#include "person.h"
#include "people_repo.h"

#include <fstream>
#include <iostream>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

const char* cachefile_name = "older_than_cache.txt";

int main(int argc, char* argv[])
{
    if (argc != 3) {
        std::cerr << "    Usage: " << argv[0] << " <person1> <person2>\n";
        return -1;
    }

    people_repo repo;
    for(person p : std::vector<person>{
            {"Jesus", "Nazareth", 33},
            {"Romario", "Rio de Janeiro", 32},
            {"John", "New York", 23},
            {"Alice", "Montevideo", 18},
            {"Hassam", "Damascus", 48},
            {"Liu", "Shangai", 38},
            {"Ngo", "Hanoi", 79}
        })
    {
        repo.new_person(std::move(p));
    }

    older_than older;
    {
        std::ifstream cache{cachefile_name};
        if (cache) {
            boost::archive::text_iarchive ar{cache};
            auto& h = ar.template get_helper<older_than_load_helper>();
            h.repo = &repo;

            ar >> older;
        }
    }

    std::string person1Name = argv[1];
    std::string person2Name = argv[2];

    auto person1 = repo.find_person(person1Name);
    if (!person1) {
        std::cerr << "Could not find anyone named " << person1Name << "\n";
        return -1;
    }

    auto person2 = repo.find_person(person2Name);
    if (!person2) {
        std::cerr << "Could not find anyone named " << person2Name << "\n";
        return -1;
    }

    if (older(*person1, *person2))
        std::cout << person1->name << " is older than " << person2->name << "\n";
    else
        std::cout << person1->name << " is not older than " << person2->name << "\n";

    std::ofstream cache{cachefile_name};

    boost::archive::text_oarchive ar{cache};
    ar << older;

    return 0;
}
