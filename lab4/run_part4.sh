#!/bin/bash
./bin/rndc -c etc/rndc.conf querylog
./part4_starter.py --ip 127.0.0.1 --port 16384 --query_port 2200
./bin/rndc -c etc/rndc.conf dumpdb -cache
# less ./named_dump.db | grep example.com