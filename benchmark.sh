#!/bin/bash
# Run C/C++ programs and creates csv file.

HT_DELTA=0.0001
LOGFILE=$PWD/log
rm -f $LOGFILE

BOOST_PREFIX=$HOME/libs
export TIMEFORMAT="%3R"

benchmark_time ()
{
   run="$*"
   echo "==================" >> $LOGFILE
   echo "* Timing $run" >> $LOGFILE
   echo "==================" >> $LOGFILE
   
   mytime=$( { time $run >> $LOGFILE; } 2>&1)
   mili=$(echo "$mytime 1000" | awk '{print $1 * $2}')
   echo $mili
}

benchmark_bp ()
{
   name="belief-propagation"
   res50=$(benchmark_time ./main 50)
   echo "$name-50 c $res50"
   res200=$(benchmark_time ./main 200)
   echo "$name-200 c $res200"
   res300=$(benchmark_time ./main 300)
   echo "$name-300 c $res300"
   res400=$(benchmark_time ./main 400)
   echo "$name-400 c $res400"
}

benchmark_ht ()
{
   name="new-heat-transfer"
   res80=$(benchmark_time ./ht ht80.data $HT_DELTA)
   echo "$name-80 c $res80"
   res120=$(benchmark_time ./ht ht120.data $HT_DELTA)
   echo "$name-120 c $res120"
}

benchmark_minimax ()
{
   name="min-max-tictactoe"
   res=$(benchmark_time ./minimax)
   echo "$name c $res"
}

benchmark_nqueens ()
{
   name="8queens"
   res11=$(benchmark_time ./nqueens 11)
   echo "$name-11 c $res11"
   res12=$(benchmark_time ./nqueens 12)
   echo "$name-12 c $res12"
   res13=$(benchmark_time ./nqueens 13)
   echo "$name-13 c $res13"
   res14=$(benchmark_time ./nqueens 14)
   echo "$name-14 c $res14"
}

benchmark_sssp ()
{
   name="shortest"
   resairports=$(benchmark_time ./dijkstra usairports.data 1)
   echo "$name-usairports500 c $resairports"
   resoclinks=$(benchmark_time ./dijkstra oclinks.data 3)
   echo "$name-oclinks c $resoclinks"
   respowergrid=$(benchmark_time ./dijkstra uspowergrid.data 5)
   echo "$name-uspowergrid c $respowergrid"
   resemail=$(benchmark_time ./dijkstra email.data 2500)
   echo "$name-email c $resemail"
}

benchmark_sssp_py ()
{
   name="shortest"
   resairports=$(benchmark_time python ./dijkstra.py ../c++/usairports.data 1)
   echo "$name-usairports500 python $resairports"
   resoclinks=$(benchmark_time python ./dijkstra.py ../c++/oclinks.data 3)
   echo "$name-oclinks python $resoclinks"
   respowergrid=$(benchmark_time python ./dijkstra.py ../c++/uspowergrid.data 5)
   echo "$name-uspowergrid python $respowergrid"
   resemail=$(benchmark_time python ./dijkstra.py ../c++/email.data 2500)
   echo "$name-email python $resemail"
}

benchmark_minimax_py ()
{
   name="min-max-tictactoe"
   res=$(benchmark_time python ./minmax.py)
   echo "$name python $res"
}

benchmark_nqueens_py ()
{
   name="8queens"
   res11=$(benchmark_time python ./nqueens.py 11)
   echo "$name-11 python $res11"
   res12=$(benchmark_time python ./nqueens.py 12)
   echo "$name-12 python $res12"
   res13=$(benchmark_time python ./nqueens.py 13)
   echo "$name-13 python $res13"
   res14=$(benchmark_time python ./nqueens.py 14)
   echo "$name-14 python $res14"
}

cd BeliefPropagation && benchmark_bp && cd .. &&
cd HeatTransfer/c++ && benchmark_ht && cd ../.. &&
cd MiniMax/c++ && benchmark_minimax && cd ../.. &&
cd NQueens/c++ && benchmark_nqueens && cd ../.. &&
cd SSSP/c++ && benchmark_sssp && cd ../..
cd MiniMax/python && benchmark_minimax_py && cd ../..
cd NQueens/python && benchmark_nqueens_py && cd ../..
cd SSSP/python && benchmark_sssp_py && cd ../..
