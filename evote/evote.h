#ifndef EVOTE_EVOTE_H
#define EVOTE_EVOTE_H

#include <eosio/eosio.hpp>
#include <string>

using namespace std;
using namespace eosio;


class [[eosio::contract("evote")]] evote : public eosio::contract {
private:
    // (1) CANDIDATES
    // Create elections status constants
    enum election_status: int8_t  {
        FINISHED   = 1,
        ONGOING     = 0,
        PREPARATION  = -1
    };

    // Candidate list
    struct candidates {
        map<string, uint64_t> candidates;  // <party, votes>
    };

    // Candidate area
    struct category_areas {
        map<string, candidates> area;  // <area, candidates>
    };

    struct election_info {
        int8_t                  status = PREPARATION;  // Status of election
        map<string, category_areas>  categories = {}; // <category, areas>
    };

    // Create a table for elections.
    struct [[eosio::table]] election {
        name             election_name;
        election_info    election_data;

        uint64_t primary_key() const { return election_name.value; }
    };

    typedef eosio::multi_index<name("election"), election> elections_table;

public:
    // Constructor
    evote( name receiver, name code, datastream<const char*> ds ):contract(receiver, code, ds) {}

    [[eosio::action]]
    void create(name election_name);

    [[eosio::action]]
    void erase(name election_name);

    [[eosio::action]]
    void newcat(name election_name, string category_name);

    [[eosio::action]]
    void delcat(name election_name,  string category_name);

    [[eosio::action]]
    void newarea(name election_name, string category_name, string area_name);

    [[eosio::action]]
    void delarea(name election_name, string category_name, string area_name);

    [[eosio::action]]
    void newcand(name election_name, string category_name, string area_name, string candidate_party);

    [[eosio::action]]
    void delcand(name election_name, string category_name, string area_name, string candidate_party);

    [[eosio::action]]
    void nextstatus(name election_name);
};


#endif //EVOTE_EVOTE_H
