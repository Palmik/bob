#/bin/bash

./build/bob/connect/server/bob_connect_server 10001 &
cd ./bob/website/bob
../env/bin/pserve development.ini &
cd ../../../
