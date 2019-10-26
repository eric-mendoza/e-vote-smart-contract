#ifndef EVOTE_VOTER_H
#define EVOTE_VOTER_H

#include <eosio/eosio.hpp>
#include <string>

using namespace std;
using namespace eosio;


class [[eosio::contract("voter")]] voter : public eosio::contract {
private:


public:
    // Constructor
    voter( name receiver, name code, datastream<const char*> ds ):contract(receiver, code, ds) {}

    [[eosio::action]]
    void addvoter(name election_name, uint64_t);

    [[eosio::action]]
    void erasevoter(name election_name, uint64_t dpi);


};


#endif //EVOTE_VOTER_H
