#!/bin/bash
cd $(dirname $0)

echo ">>> start simulation";
SECONDS=0;
for i in {0..9}; do
    start=$SECONDS;
    ./hypercolumn "$i"
    loop_time=$SECONDS-$start
    echo ">>> Iteration ${i} (${loop_time}s)";
done
echo ">>> simulation time: ${SECONDS}s";
