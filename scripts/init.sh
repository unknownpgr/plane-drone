#!/bin/bash
cd `dirname $0`/..
BASE_DIR=`pwd`/..

sudo apt update -y
sudo apt install -y         \
          cmake             \
          gcc-arm-none-eabi \
          build-essential   \
          libnewlib-arm-none-eabi

cd /usr/local
git clone https://github.com/raspberrypi/pico-sdk.git --branch master
cd pico-sdk
git submodule update --init

