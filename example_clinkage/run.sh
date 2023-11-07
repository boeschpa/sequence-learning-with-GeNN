#!/bin/bash
rm -r tenHHRing_CODE
rm generator
rm generator.d
rm Makefile
rm output.V.dat
rm output.spikes.dat 
rm stimulator

cd $(dirname $0)
genn-buildmodel.sh -c model.cc;
genn-create-user-project.sh example simulation.cc;
make;
./example;
python3 ../util/plot_output.py output.V.dat&
python3 ../util/plot_spikes.py output.spikes.dat &
wait