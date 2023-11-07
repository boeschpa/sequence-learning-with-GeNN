#!/bin/bash
cd $(dirname $0)
rm -r hypercolumns_CODE
rm generator
rm generator.d
rm hypercolumns
rm Makefile
rm output.spikes.dat
rm output.V.dat

genn-buildmodel.sh -cf model.cc;
genn-create-user-project.sh hypercolumns simulation.cc;
make;
./hypercolumns;
#python3 ../util/plot_output.py output.V.dat&
#python3 ../util/plot_spikes.py output.spikes.dat &
#wait