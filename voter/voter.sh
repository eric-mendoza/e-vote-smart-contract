#!/bin/sh
eosio-cpp voter.cpp -o voter.wasm -I . -I ../evote/
cleos set contract voter ./ -p voter@active
cleos push action voter vote '[1,"DE507F0B2F4170D814922A2F29219D5C8936BBBF3ED460D68824CC2270455164","eleccion1", "ciudad guatemala", [{"key":"presidente", "value":"une"},{"key":"alcalde", "value":"une"}]]' -p generaluser@active
