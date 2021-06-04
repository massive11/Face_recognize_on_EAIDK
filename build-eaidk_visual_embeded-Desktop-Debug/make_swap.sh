#!/bin/sh

sudo dd if=/dev/zero of=/swapfile bs=64M count=16
sudo mkswap /swapfile
sudo chmod 600 /swapfile
sudo swapon /swapfile
