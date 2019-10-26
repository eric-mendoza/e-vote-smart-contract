#include "evote.h"

// ELECTION ACTIONS
void evote::create(name election_name) {
    // Only TSE can create an election
    require_auth(name("tse"));

    // Instantiate multi-index table aka: Get the first row
    // (1) Owner of table. (2) Account name hits contract deployed to
    //      get_self() gets the name of the contract (evote)
    elections_table elections(get_self(), get_first_receiver().value);

    // Next, query the table for the election.
    auto row = elections.find(election_name.value);

    // Check if election doesn't exists
    check(row == elections.end(), "Election name already taken.");

    // Create election info
    election_info info = {};

    elections.emplace(get_self(), [&]( auto& row){
        row.election_name = election_name;
        row.election_data = info;
    });
}

void evote::erase(name election_name) {
    // Only tse can delete an election
    require_auth(name("tse"));

    // Get election row
    elections_table elections(get_self(), get_first_receiver().value);
    auto row = elections.find(election_name.value);
    check(row != elections.end(), "Election doesn't exists");

    // Changes can only be made when election is in PREPARATION status
    election_info election_data = row->election_data;
    check(election_data.status == PREPARATION, "Election is not longer in PREPARATION status.");

    // Erase
    elections.erase(row);
}

void evote::nextstatus(name election_name) {
    // Only tse can delete an election
    require_auth(name("tse"));

    // Get election row
    elections_table elections(get_self(), get_first_receiver().value);
    auto row = elections.find(election_name.value);
    check(row != elections.end(), "Election doesn't exists");

    // Change status. Status can only move forward.
    election_info election_data = row->election_data;
    check(election_data.status != FINISHED, "Election is already FINISHED.");

    election_data.status++;

    // Save to multi index table
    elections.modify(row, get_self(), [&](auto& row){
        row.election_name = election_name;
        row.election_data = election_data;
    });
}

// CANDIDATES ACTIONS
// This action adds a New Candidate to the blockchain in a given election.
// Params:
//      - election_name
//      - candidate_type
//      - candidate_party
void evote::newcand(name election_name, int candidate_type, string candidate_party){
    // Only tse can delete an election
    require_auth(name("tse"));

    // Get election row
    elections_table elections(get_self(), get_first_receiver().value);
    auto row = elections.find(election_name.value);
    check(row != elections.end(), "Election doesn't exists");

    // Get election_data
    election_info election_data = row->election_data;

    // Changes can only be made when election is in PREPARATION status
    check(election_data.status == PREPARATION, "Election must be in PREPARATION status to be changed.");

    // Get candidates of category requested
    candidate_list* category_candidates = &election_data.categories[candidate_type];

    // See if candidate already exists
    for (auto &candidate : category_candidates->candidates) {
        check(candidate.party != candidate_party,
                "Party '" + candidate_party + "' already has a candidate for '" + candidate_type_dict[candidate_type] + "'.");
    }

    // Add candidate if it doesn't exists
    candidate new_candidate = {candidate_party, 0};
    category_candidates->candidates.push_back(new_candidate);

    // Save to multi index table
    elections.modify(row, get_self(), [&](auto& row){
        row.election_name = election_name;
        row.election_data = election_data;
    });

}

void evote::delcand(name election_name, int candidate_type, string candidate_party){
    // Only tse can delete an election
    require_auth(name("tse"));

    // Get election row
    elections_table elections(get_self(), get_first_receiver().value);
    auto row = elections.find(election_name.value);
    check(row != elections.end(), "Election doesn't exists");

    // Get election_data
    election_info election_data = row->election_data;

    // Changes can only be made when election is in PREPARATION status
    check(election_data.status == PREPARATION, "Election must be in PREPARATION status to be changed.");

    // Get candidates of category requested
    candidate_list* category_candidates = &election_data.categories[candidate_type];

    // Find candidate
    bool modified = false;
    auto iter = category_candidates->candidates.begin();
    while (iter != category_candidates->candidates.end()) {
        if (iter->party == candidate_party) {
            modified = true;
            print("ELIMINADO");
            category_candidates->candidates.erase(iter);
            break;
        } else {
            ++iter;
        }
    }

    check(modified, "Candidate doesn't exists. Didn't change anything.");

    elections.modify(row, get_self(), [&](auto& row){
        row.election_name = election_name;
        row.election_data = election_data;
    });
}