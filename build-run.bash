#!/bin/bash
cd src
set -u -e
make opt
cd ../bin && ./hello
