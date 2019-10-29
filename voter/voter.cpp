#include "voter.h"

[[eosio::action]]
void voter::addvoter(name election_name, uint64_t) {
    print("Added.");
};

[[eosio::action]]
void voter::erasevoter(name election_name, uint64_t dpi) {
    print("Erased");
};

[[eosio::action]]
void voter::vote(name election_name, uint64_t dpi, string presidente, string alcalde) {
    print("voted");
};