#!/bin/bash -e

apt clean
apt update
apt install -y gcc make build-essential linux-headers-$(uname -r)

cd /vagrant

make clean
make test 
make module

set +e
rmmod zombie_seeker
set -e

insmod zombie_seeker.ko

exit 0