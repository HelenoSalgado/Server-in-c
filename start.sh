#!/bin/bash

gcc -Wall -Wreturn-type -Wextra *.c ./http/*.c ./http/get/*.c ./utils/*.c -o ./bin/server &&

sudo ./bin/server