#!/bin/sh
eosio-cpp evote.cpp -o evote.wasm -I .
cleos set contract evote ./ -p evote@active
cleos push action evote delcand '["eleccion1","presidente","nacional","borrar ete"]' -p tse@active
