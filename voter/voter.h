#ifndef EVOTE_VOTER_H
#define EVOTE_VOTER_H

#include <eosio/eosio.hpp>
#include <string>

using namespace std;
using namespace eosio;


class [[eosio::contract("voter")]] voter : public eosio::contract {
private:
    // Create a table for voters.
    struct [[eosio::table]] voters {
        uint64_t        dpi;
        bool            voted;

        uint64_t primary_key() const { return dpi; }
    };

    typedef eosio::multi_index<name("voters"), election> voters_table;


public:
    // Constructor
    voter( name receiver, name code, datastream<const char*> ds ):contract(receiver, code, ds) {}

    [[eosio::action]]
    void addvoter(name election_name, uint64_t);

    [[eosio::action]]
    void erasevoter(name election_name, uint64_t dpi);

    [[eosio::action]]
    void vote(name election_name, uint64_t dpi, string presidente, string alcalde);


};


#endif //EVOTE_VOTER_H
