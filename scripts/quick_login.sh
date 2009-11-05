#!/bin/bash

echo "This will set up quick login on the vehicle"
if [ ! -e "$HOME/.ssh/id_rsa" ]; then
    echo "No rsa key, generating one..."
    echo "Press enter 3 times to use the default settings (this script won't work if you don't use the default settings!)"
    ssh-keygen -t rsa
fi

echo "Enter tortuga3 password"
idrsa=$(cat ~/.ssh/id_rsa.pub)
ssh jlisee@tortuga3 ./ram_code/scripts/register_computer.sh $idrsa

# Check the .bashrc file for the string "alias ssh-hack"
# Does not check for scp-hack, just assumes it is there if ssh-hack is there
sshhack=$(cat ~/.bashrc | grep -e "alias ssh-hack" | wc -l)

if [ $sshhack -ne 0 ]; then
    echo "The hack is already on this computer. Skipping so the bashrc file doesn't get convoluted."
else
    echo "alias ssh-hack='SSH_AUTH_SOCK=0 ssh'" | cat >> ~/.bashrc
    echo "alias scp-hack='SSH_AUTH_SOCK=0 scp'" | cat >> ~/.bashrc
    echo "ssh public keys have been shared. Setting up the hack to make it work."

    # Rerun the bashrc file
    source ~/.bashrc
fi
