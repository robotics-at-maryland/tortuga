#!/bin/bash -l

for ((i = 0 ; i < $1 ; i ++))
do build/python/testclient.py
done

