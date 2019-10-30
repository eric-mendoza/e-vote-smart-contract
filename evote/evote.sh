#!/bin/sh
eosio-cpp evote.cpp -o evote.wasm -I .
cleos set contract evote ./ -p evote@active
