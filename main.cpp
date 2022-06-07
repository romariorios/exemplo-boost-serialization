#include "older_than.h"
#include "older_than_serialization.h"
#include "person.h"
#include "people_repo.h"

#include <fstream>
#include <functional>
#include <iostream>
#include <variant>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <eos/portable_iarchive.hpp>
#include <eos/portable_oarchive.hpp>

const char* boost_cachefile_name = "older_than_cache.txt";
const char* eos_cachefile_name = "older_than_cache.eos.bin";

struct boost_archive_option {};
struct eos_archive_option {};
using archive_option = std::variant<boost_archive_option, eos_archive_option>;

template <typename Action>
void with_iarchive(boost_archive_option, Action action)
{
    std::ifstream cache{boost_cachefile_name};
    if (!cache) return;

    boost::archive::text_iarchive ar{cache};
    action(ar);
}

template <typename Action>
void with_iarchive(eos_archive_option, Action action)
{
    std::ifstream cache{eos_cachefile_name, std::ios::binary};
    if (!cache) return;

    eos::portable_iarchive ar{cache};
    action(ar);
}

template <typename Action>
void with_iarchive(archive_option opt, Action action)
{
    std::visit([&](auto&& o){ with_iarchive(o, action); }, opt);
}

template <typename Action>
void with_oarchive(boost_archive_option, Action action)
{
    std::ofstream cache{boost_cachefile_name};
    boost::archive::text_oarchive ar{cache};
    action(ar);
}

template <typename Action>
void with_oarchive(eos_archive_option, Action action)
{
    std::ofstream cache{eos_cachefile_name, std::ios::binary};
    eos::portable_oarchive ar{cache};
    action(ar);
}

template <typename Action>
void with_oarchive(archive_option opt, Action action)
{
    std::visit([&](auto&& o){ with_oarchive(o, action); }, opt);
}

int main(int argc, char* argv[])
{
    if (argc != 4) {
        std::cerr
            << "    Usage: " << argv[0] << " <archive-type> <person1> <person2>\n"
            << "      archive-type: \"text\", \"eos\"\n";
        return -1;
    }

    std::string optString = argv[1];
    std::string person1Name = argv[2];
    std::string person2Name = argv[3];

    archive_option opt;
    if (optString == "text")
        opt = boost_archive_option{};
    else if (optString == "eos")
        opt = eos_archive_option{};
    else {
        std::cerr << "Invalid archive-type: " << optString << "\n";
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
    with_iarchive(opt, [&](auto& ar)
    {
        auto& h = ar.template get_helper<older_than_load_helper>();
        h.repo = &repo;

        ar >> older;
    });

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

    with_oarchive(opt, [&](auto& ar)
    {
        ar << older;
    });

    return 0;
}
