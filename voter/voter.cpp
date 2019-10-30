#include "voter.h"

[[eosio::action]]
void voter::addvoter(uint64_t id, checksum256 dpi, name election_name, uint64_t mesa_id) {
    // Only TSE can add voters
    require_auth(name("tse"));

    // Assert election exists
    elections_table elections(name("evote"), name("evote").value);
    auto election_row = elections.find(election_name.value);
    check(election_row != elections.end(), "Election doesn't exists");

    // Assert election is in PREPARATION status
    election_info election_data = election_row->election_data;
    check(election_data.status == PREPARATION, "Election must be in PREPARATION status to edit voters.");


    // Instantiate voters table
    voters_table voters(get_self(), get_first_receiver().value);
    auto row = voters.find(id);
    check(row == voters.end(), "Voter already exists.");

    voters.emplace(get_self(), [&]( auto& row){
        row.id = id;
        row.dpi = dpi;
        row.election_name = election_name;
        row.mesa_id = mesa_id;
    });
};

[[eosio::action]]
void voter::erasevoter(uint64_t id, checksum256 dpi, name election_name) {
    // Only TSE can add voters
    require_auth(name("tse"));

    // Assert election exists
    elections_table elections(name("evote"), name("evote").value);
    auto election_row = elections.find(election_name.value);
    check(election_row != elections.end(), "Election doesn't exists");

    // Assert election is in PREPARATION status
    election_info election_data = election_row->election_data;
    check(election_data.status == PREPARATION, "Election must be in PREPARATION status to edit voters.");

    // Instantiate voters table
    voters_table voters(get_self(), get_first_receiver().value);
    auto row = voters.find(id);
    check(row != voters.end(), "Voter doesn't exists.");
    check(row->dpi == dpi, "DPI hash mismatch.");
    check(row->election_name == election_name, "Election name mismatch.");

    voters.erase(row);
};

[[eosio::action]]
void voter::vote(uint64_t id, checksum256 dpi, name election_name, map<string, string> candidates) {
    // Users will use a generic account to vote
    print("voted");
};