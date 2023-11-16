#!/bin/bash
cd $(dirname $0)
rm -r hypercolumn_CODE
rm generator
rm generator.d
rm hypercolumn
rm Makefile
rm output.spikes.dat
rm output.V.dat

genn-buildmodel.sh -cf model.cc;
python3 generate_code_interface.py
genn-create-user-project.sh hypercolumn simulation.cc;
make;
./hypercolumn;
python3 ../util/plot_spikes_new.py output.spikes.csv -noshow
code -r plot_spikes_hypercolumn.png;