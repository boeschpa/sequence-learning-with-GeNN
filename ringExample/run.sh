#!/bin/bash
rm -r tenHHRing_CODE
rm generator
rm generator.d
rm Makefile
rm tenHH_output.V.dat
rm tenHH_output.spikes.dat 
rm tenHHRing

cd $(dirname $0)
genn-buildmodel.sh -c tenHHRingModel.cc;
genn-create-user-project.sh tenHHRing tenHHRingSimulation.cc;
make;
./tenHHRing;
python3 ../util/plot_output.py tenHH_output.V.dat&
python3 ../util/plot_spikes.py tenHH_output.spikes.dat &
wait