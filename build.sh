#!/usr/bin/env bash

mkdir tmp && cd tmp && cmake .. && make
cd .. && rm -rf tmp
