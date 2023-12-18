#!/bin/bash
cd $(dirname $0)

echo ">>> start simulation";
SECONDS=0;
for l in {5..5..30}; do
    for i in {0..9}; do
        start=$SECONDS;
        ./hypercolumn "$i" "$l"
        loop_time=$((SECONDS-start))
        echo ">>> Sequence length $(l), iteration ${i} (${loop_time}s)";
    done
done
echo ">>> simulation time: ${SECONDS}s";
