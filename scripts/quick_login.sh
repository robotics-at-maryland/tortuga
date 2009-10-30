#!/bin/bash

echo "This will set up quick login on the vehicle"
if [ ! -e "$HOME/.ssh/id_rsa" ]; then
    echo "No rsa key, generating one..."
    echo "Press enter 3 times to use the default settings (this script won't work if you don't use the default settings!)"
    ssh-keygen -t rsa
fi

echo "Enter tortuga3 password"
ssh jlisee@192.168.10.11 ./ram_code/scripts/register_computer.sh | cat ~/.ssh/id_rsa.pub

if [ cat ~/.bashrc contains "alias ssh-hack" ]; then
    echo "The hack is already on this computer. Skipping so the bashrc file doesn't get convoluted."
else
    echo "alias ssh-hack='SSH_AUTH_SOCK=0 ssh'" | cat >> ~/.bashrc
    echo "alias scp-hack='SSH_AUTH_SOCK=0 scp'" | cat >> ~/.bashrc
    echo "ssh public keys have been shared. Setting up the hack to make it work."
fi
