#!/bin/bash
cd src
set -u -e
make dbg
cd ../bin && ./hellodbg
