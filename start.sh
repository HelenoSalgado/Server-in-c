#!/bin/bash

gcc -Wall *.c ./http/*.c -o ./bin/server && sudo ./bin/server