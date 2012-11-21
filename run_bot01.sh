#/bin/bash

RUNB="./bob/gomoku/bots/random_bot"
RUNC="./build/bob/client/bob_client $1 $2"

rm     /tmp/fifo{B,C}1 &> /dev/null
mkfifo /tmp/fifo{B,C}1
cat /tmp/fifoC1 | $RUNC | tee /tmp/fifoB1 &
cat /tmp/fifoB1 | $RUNB | tee /tmp/fifoC1 &

