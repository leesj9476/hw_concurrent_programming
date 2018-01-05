#!/bin/sh
echo y | ./run/bin/mysqladmin -uroot drop sbtest
./run/bin/mysqladmin -uroot create sbtest
sysbench --threads=100 --rand-type=uniform --mysql-user=root --mysql-socket=./run/mariadb.sock --db-driver=mysql oltp_read_only.lua prepare
sysbench --threads=100 --rand-type=uniform --mysql-user=root --mysql-socket=./run/mariadb.sock --db-driver=mysql oltp_read_only.lua run 
sysbench --threads=100 --rand-type=uniform --mysql-user=root --mysql-socket=./run/mariadb.sock --db-driver=mysql oltp_read_only.lua cleanup
