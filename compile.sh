#!/bin/bash

if [ -z "$BOOST_PREFIX" ]; then
   echo "BOOST_PREFIX not set!"
fi

cd BeliefPropagation &&
INCLUDES=-I$BOOST_PREFIX/include make clean all &&
cd .. &&
cd HeatTransfer/c++ &&
make clean all &&
cd ../.. &&
cd MiniMax/c++ &&
make clean all &&
cd ../.. &&
cd NQueens/c++ &&
make clean all &&
cd ../.. &&
cd SSSP/c++ &&
make clean all &&
cd ../..
