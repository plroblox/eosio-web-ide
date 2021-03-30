#!/bin/bash
# Run this script in /workspace/eosio-web-ide on a new terminal of your gitpod environment
# Use 'chmod +x test1.sh' to make this .sh script executable, then do
#       ./test1.sh
#
# If you want to have a fresh start after say modifying your contract code ect, you may want to restart nodoes,
# then ***BEFORE you run this script, you may want to go to the terminal running nodeos to kill nodeos by CTRL +C,
# and then run ./restartNodeos.sh


echo "A trivial test script for EOSIO Web IDE project"

echo "=== Build wasm and abi files..."
eosio-cpp contract/talk.cpp

echo "=== Install contract talk..."
cleos create account eosio talk EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
cleos set code talk talk.wasm
cleos set abi talk talk.abi

echo "=== Create test accounts..."
cleos create account eosio bob EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
cleos create account eosio jane EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
cleos create account eosio peter EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
cleos create account eosio tom EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV

echo "=== Verify action post..."
cleos push action talk post '[1000, 0, bob, "This is a new post", 10, 0]' -p bob
cleos push action talk post '[2000, 0, jane, "This is my first post", 20,1]' -p jane
cleos push action talk post '[1001, 2000, bob, "Replying to your post", 20,10]' -p bob

echo "=== Verify action addlikes and adddislikes..."
cleos push action talk addlikes '[1001]' -p peter
cleos push action talk adddislikes '[1001]' -p tom

echo "=== Show all rows in the multi-index table... "
cleos get table talk '' message
echo "------>Check the row with id=1001 has #likes=21 and #dislikes=11"



