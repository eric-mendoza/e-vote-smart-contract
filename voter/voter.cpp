#include "voter.h"

void voter::addvoter(uint64_t id, checksum256 dpi, name election_name, uint64_t mesa_id) {
    // Only TSE can add voters
    require_auth(name("tse"));

    // Assert election exists
    evote::elections_table elections(name("evote"), name("evote").value);
    auto election_row = elections.find(election_name.value);
    check(election_row != elections.end(), "Election doesn't exists");

    // Assert election is in PREPARATION status
    evote::election_info election_data = election_row->election_data;
    check(election_data.status == evote::PREPARATION, "Election must be in PREPARATION status to edit voters.");


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

void voter::erasevoter(uint64_t id, checksum256 dpi, name election_name) {
    // Only TSE can add voters
    require_auth(name("tse"));

    // Assert election exists
    evote::elections_table elections(name("evote"), name("evote").value);
    auto election_row = elections.find(election_name.value);
    check(election_row != elections.end(), "Election doesn't exists");

    // Assert election is in PREPARATION status
    evote::election_info election_data = election_row->election_data;
    check(election_data.status == evote::PREPARATION, "Election must be in PREPARATION status to edit voters.");

    // Instantiate voters table
    voters_table voters(get_self(), get_first_receiver().value);
    auto row = voters.find(id);
    check(row != voters.end(), "Voter doesn't exists.");
    check(row->dpi == dpi, "DPI hash mismatch.");
    check(row->election_name == election_name, "Election name mismatch.");

    voters.erase(row);
};

void voter::vote(uint64_t id, checksum256 dpi, name election_name, string user_area, map<string, string> candidates) {
    // Only TSE can add voters
    require_auth(name("generaluser"));

    // Assert election exists
    evote::elections_table elections(name("evote"), name("evote").value);
    auto election_row = elections.find(election_name.value);
    check(election_row != elections.end(), "Election doesn't exists");

    // Assert election is in PREPARATION status
    evote::election_info election_data = election_row->election_data;
    check(election_data.status == evote::PREPARATION, "Election must be in ONGOING status to emit votes.");

    // Instantiate voters table
    voters_table voters(get_self(), get_first_receiver().value);
    auto row = voters.find(id);
    check(row != voters.end(), "Voter doesn't exists.");
    check(row->dpi == dpi, "DPI hash mismatch.");
    check(row->election_name == election_name, "Election name mismatch.");

    // Send votes
    voter::send_vote(election_name, user_area, candidates);
};

void voter::send_vote(name election_name, string user_area, map<string, string> user_votes){
    // Parameters: (1) Smart contract to be called
    evote::vote_action newvote(name("evote"), {get_self(), name("active")});
    newvote.send(election_name, user_area, user_votes);
}