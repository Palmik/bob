#/bin/bash

./build/bob/gomoku/server/bob_gomoku_server 10001 &
cd ./bob/website/bob
../env/bin/pserve development.ini &
cd ../../../
