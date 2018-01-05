#!/bin/sh
cd server
make install
cd ../run
./scripts/mysql_install_db --datadir=../data
cd ..
