#!/bin/bash
# Run this script in the terminal running nodeos after using CTRL+C to kill the running nodeos
# Please use this script in the GitPod environment
# Use 'chmod +x restartNodeos.sh' to make this .sh script executable, then do
#       ./restartNodeos.sh

echo "=== Restart Nodeos..."
rm -rf ~/eosio/chain
nodeos --config-dir ~/eosio/chain/config --data-dir ~/eosio/chain/data -e -p eosio --plugin eosio::chain_api_plugin --contracts-console
