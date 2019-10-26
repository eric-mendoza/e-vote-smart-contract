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

    // Candidate type
    enum candidate_type: uint8_t {
        PRESIDENTE = 0,
        ALCALDE = 1,
        DIPUTADO_NACIONAL = 2,
        DIPUTADO_REGIONAL = 3,
        PARLACEN = 4,
    };

    const string candidate_type_dict[5] = {
            "Presidente",
            "Alcalde",
            "Diputado listado nacional",
            "Diputado regional",
            "Parlacen"};

    // Candidates per party
    struct candidate {
        string party;
        uint64_t votes;
    };

    // Candidate list
    struct candidate_list {
        vector<candidate> candidates;
    };

    // This is going to save the results of a given election eg: first or second round
    struct election_info {
        int8_t              status = PREPARATION;  // Status of election
        vector<candidate_list> categories = {
                {},  // President
                {},  // Major
                {},  // Diputados Nacional
                {},  // Diputados regional
                {},  // Parlacen
        };
    };

    // (2) ELECTIONS
    // Create a table for elections.
    // Here we are going to save the first and second voting rounds
    struct [[eosio::table]] election {
        name        election_name;
        election_info    election_data;

        uint64_t primary_key() const { return election_name.value; }
    };

    typedef eosio::multi_index<name("election"), election> elections_table;


    // VOTERS
    // Set user table structure
    struct [[eosio::table]] user_info {
        name            dpi_finger;  // Huella dactilar cifrada en dpi
        uint16_t        voted = 0;

        auto primary_key() const { return dpi_finger.value; }  // Primary key is dactilar info
    };

    typedef eosio::multi_index<name("users"), user_info> users_table;  // Define data type for table

public:
    // Constructor
    evote( name receiver, name code, datastream<const char*> ds ):contract(receiver, code, ds) {}

    [[eosio::action]]
    void create(name election_name);

    [[eosio::action]]
    void erase(name election_name);

    [[eosio::action]]
    void newcand(name election_name, int candidate_type, string candidate_party);

    [[eosio::action]]
    void delcand(name election_name, int candidate_type, string candidate_party);

    [[eosio::action]]
    void nextstatus(name election_name);
};


#endif //EVOTE_EVOTE_H
