#!/bin/bash
cd $(dirname $0)
genn-buildmodel.sh -c model.cc;
genn-create-user-project.sh hypercolumns simulation.cc;
make;
./hypercolumns;
python3 ../util/plot_output.py output.V.dat&
python3 ../util/plot_spikes.py output.spikes.dat &
wait