#/bin/bash

RUNB="./bob/gomoku/bots/random_bot"
RUNC="./build/bob/client/bob_client $1 $2"

rm     /tmp/fifo{B,C}2 &> /dev/null
mkfifo /tmp/fifo{B,C}2
cat /tmp/fifoC2 | $RUNC | tee /tmp/fifoB2 &
cat /tmp/fifoB2 | $RUNB | tee /tmp/fifoC2 &

