#!/bin/sh
eosio-cpp voter.cpp -o voter.wasm -I .
cleos set contract voter ./ -p voter@active