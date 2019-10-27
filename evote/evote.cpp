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
    check(row == elections.end(), "Election name already exists.");

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
    //check(election_data.status == PREPARATION, "Election is not longer in PREPARATION status.");  // todo: UNCOMMENT THIS

    // Erase
    elections.erase(row);
}


void evote::newcat(name election_name, string category_name){
    require_auth(name("tse"));

    // Get election row
    elections_table elections(get_self(), get_first_receiver().value);
    auto row = elections.find(election_name.value);
    check(row != elections.end(), "Election doesn't exists");

    // Get election_data
    election_info election_data = row->election_data;

    // Assert election is in PREPARATION status
    check(election_data.status == PREPARATION, "Election must be in PREPARATION status to be changed.");

    // Assert category doesn't exists
    auto category = election_data.categories.find(category_name);
    check(category == election_data.categories.end(), "Category already exists");

    // Create new category
    category_areas areas = {};
    election_data.categories.insert({category_name, areas});


    // Save to multi index table
    elections.modify(row, get_self(), [&](auto& row){
        row.election_name = election_name;
        row.election_data = election_data;
    });
}


void evote::delcat(name election_name, string category_name){
    require_auth(name("tse"));

    // Get election row
    elections_table elections(get_self(), get_first_receiver().value);
    auto row = elections.find(election_name.value);
    check(row != elections.end(), "Election doesn't exists");

    // Get election_data
    election_info election_data = row->election_data;

    // Assert election is in PREPARATION status
    check(election_data.status == PREPARATION, "Election must be in PREPARATION status to be changed.");

    // Assert category exists
    auto category = election_data.categories.find(category_name);
    check(category != election_data.categories.end(), "Category doesn't exists");

    // Delete category
    election_data.categories.erase(category);

    // Save to multi index table
    elections.modify(row, get_self(), [&](auto& row){
        row.election_name = election_name;
        row.election_data = election_data;
    });
}


void evote::newarea(name election_name, string category_name, string area_name){
    require_auth(name("tse"));

    // Get election row
    elections_table elections(get_self(), get_first_receiver().value);
    auto row = elections.find(election_name.value);
    check(row != elections.end(), "Election doesn't exists");

    // Get election_data
    election_info election_data = row->election_data;

    // Assert election is in PREPARATION status
    check(election_data.status == PREPARATION, "Election must be in PREPARATION status to be changed.");

    // Assert category exists
    auto category = election_data.categories.find(category_name);
    check(category != election_data.categories.end(), "Category doesn't exists");

    // Assert area doesnt exists
    map<string, candidates> category_area = category->second.area;
    auto area = category_area.find(area_name);
    check(area == category_area.end(), "Area already exists");

    // Create new area
    candidates candidate_list = {};
    category->second.area.insert({area_name, candidate_list});

    // Save to multi index table
    elections.modify(row, get_self(), [&](auto& row){
        row.election_name = election_name;
        row.election_data = election_data;
    });
}


void evote::delarea(name election_name, string category_name, string area_name){
    require_auth(name("tse"));

    // Get election row
    elections_table elections(get_self(), get_first_receiver().value);
    auto row = elections.find(election_name.value);
    check(row != elections.end(), "Election doesn't exists");

    // Get election_data
    election_info election_data = row->election_data;

    // Assert election is in PREPARATION status
    check(election_data.status == PREPARATION, "Election must be in PREPARATION status to be changed.");

    // Assert category exists
    auto category = election_data.categories.find(category_name);
    check(category != election_data.categories.end(), "Category doesn't exists");

    // Assert area exists
    map<string, candidates> category_area = category->second.area;
    auto area = category_area.find(area_name);
    check(area != category_area.end(), "Category doesn't exists");

    // Delete area
    category->second.area.erase(area_name);

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
void evote::newcand(name election_name, string category_name, string area_name, string candidate_party){
    // Only tse can delete an election
    require_auth(name("tse"));

    // Get election row
    elections_table elections(get_self(), get_first_receiver().value);
    auto row = elections.find(election_name.value);
    check(row != elections.end(), "Election doesn't exists");

    // Get election_data
    election_info election_data = row->election_data;

    // Assert election is in PREPARATION status
    check(election_data.status == PREPARATION, "Election must be in PREPARATION status to be changed.");

    // Assert category exists
    auto category = election_data.categories.find(category_name);
    check(category != election_data.categories.end(), "Category doesn't exists");

    // Assert area exists
    map<string, candidates>* category_area = &category->second.area;
    auto area = category_area->find(area_name);
    check(area != category_area->end(), "Area doesn't exists");

    // Assert candidate doesnt exists
    map<string, uint64_t> candidates = area->second.candidates;
    auto candidate = candidates.find(candidate_party);
    check(candidate == candidates.end(), "Candidate already exists");

    // Create new candidate
    area->second.candidates.insert({candidate_party, 0});

    // Save to multi index table
    elections.modify(row, get_self(), [&](auto& row){
        row.election_name = election_name;
        row.election_data = election_data;
    });

}

void evote::delcand(name election_name, string category_name, string area_name, string candidate_party){
    // Only tse can delete an election
    require_auth(name("tse"));

    // Get election row
    elections_table elections(get_self(), get_first_receiver().value);
    auto row = elections.find(election_name.value);
    check(row != elections.end(), "Election doesn't exists");

    // Get election_data
    election_info election_data = row->election_data;

    // Assert election is in PREPARATION status
    check(election_data.status == PREPARATION, "Election must be in PREPARATION status to be changed.");

    // Assert category exists
    auto category = election_data.categories.find(category_name);
    check(category != election_data.categories.end(), "Category doesn't exists");

    // Assert area exists
    map<string, candidates>* category_area = &category->second.area;
    auto area = category_area->find(area_name);
    check(area != category_area->end(), "Area doesn't exists");

    // Assert candidate exists
    map<string, uint64_t> candidates = area->second.candidates;
    auto candidate = candidates.find(candidate_party);
    check(candidate != candidates.end(), "Candidate doesn't exists");

    // Create new candidate
    area->second.candidates.erase(candidate_party);

    elections.modify(row, get_self(), [&](auto& row){
        row.election_name = election_name;
        row.election_data = election_data;
    });
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
