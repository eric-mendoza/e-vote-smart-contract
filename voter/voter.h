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
        name            election_name;
        uint64_t        mesa_id;  // This is only saved in API db
        bool            voted = false;
        bool            enabled = false;  // This is enabled before voting

        // Primary key
        uint64_t primary_key() const { return id; }

        // Secondary key
        checksum256 by_dpi() const { return get_dpi(dpi); }

        // Secondary key
        uint64_t by_election() const { return election_name.value; }

        // Convert sha256 to uint64_t
        static checksum256 get_dpi(const checksum256 &dpi) {
            const uint64_t *p64 = reinterpret_cast<const uint64_t *>(&dpi);
            return checksum256::make_from_word_sequence<uint64_t>(p64[0], p64[1], p64[2], p64[3]);
        }
    };

    typedef eosio::multi_index<name("voters"), voters,
            indexed_by<name("bydpi"), const_mem_fun<voters, checksum256, &voters::by_dpi>>,
            indexed_by<name("byelection"), const_mem_fun<voters, uint64_t, &voters::by_election>>
            > voters_table;

    // Election multi-index data
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
    voter(name receiver, name code, datastream<const char*> ds ):contract(receiver, code, ds) {}

    [[eosio::action]]
    void addvoter(uint64_t id, checksum256 dpi, name election_name, uint64_t mesa_id);

    [[eosio::action]]
    void erasevoter(uint64_t id, checksum256 dpi, name election_name);

    [[eosio::action]]
    void vote(uint64_t id, checksum256 dpi, name election_name, map<string, string> candidates);
};
#endif //EVOTE_VOTER_H
