#!/bin/bash

echo "Press enter 3 times"
ssh-keygen -t rsa
echo "Enter tortuga3 password twice"
ssh jlisee@192.168.10.11 mkdir -p .ssh
~/.ssh/id_rsa.pub | ssh jlisee@192.168.10.11 'cat >> .ssh/authorized_keys'
ssh jlisee@192.168.10.11
