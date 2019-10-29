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
        uint64_t        id;
        checksum256     dpi;  // cifrado
        uint64_t        mesa_id;  // This is only saved in API db
        bool            voted = false;
        bool            enabled = false;  // This is enabled before voting

        // Primary key
        uint64_t primary_key() const { return id; }

        // Secondary key
        checksum256 by_dpi() const { return get_dpi(dpi); }

        // Convert sha256 to uint64_t
        static checksum256 get_dpi(const checksum256 &dpi) {
            const uint64_t *p64 = reinterpret_cast<const uint64_t *>(&dpi);
            return checksum256::make_from_word_sequence<uint64_t>(p64[0], p64[1], p64[2], p64[3]);
        }
    };

    typedef eosio::multi_index<name("voters"), voters,
            indexed_by<name("bydpi"), const_mem_fun<voters, checksum256, &voters::by_dpi>>
            > voters_table;

public:
    // Constructor
    voter(name receiver, name code, datastream<const char*> ds ):contract(receiver, code, ds) {}

    [[eosio::action]]
    void addvoter(name election_name, uint64_t);

    [[eosio::action]]
    void erasevoter(name election_name, uint64_t dpi);

    [[eosio::action]]
    void vote(name election_name, uint64_t dpi, string presidente, string alcalde);
};
#endif //EVOTE_VOTER_H
