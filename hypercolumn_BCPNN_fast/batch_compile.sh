#!/bin/bash
cd $(dirname $0)
rm -r hypercolumn_CODE
rm generator
rm generator.d
rm hypercolumn
rm Makefile
rm output.spikes.csv
rm code_interface.h

genn-buildmodel.sh -f model.cc;
python3 generate_code_interface.py
genn-create-user-project.sh hypercolumn simulation.cc;
echo ">>> start compilation";
SECONDS=0;
make;
echo ">>> compile time: ${SECONDS}s";

current_datetime=$(date "+%Y%m%d_%H%M%S")
directory_name="batch_$current_datetime"
mkdir "$directory_name"
cp hypercolumn "$directory_name"
cp model_param.h "$directory_name"
cp batch_run.sh "$directory_name"
cp analyse_batch.py "$directory_name"
cp -r hypercolumn_CODE "$directory_name" 