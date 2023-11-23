#!/bin/bash
cd $(dirname $0)
rm -r hypercolumn_CODE
rm generator
rm generator.d
rm hypercolumn
rm Makefile
rm output.spikes.csv

genn-buildmodel.sh -cf model.cc;
python3 generate_code_interface.py
genn-create-user-project.sh hypercolumn simulation.cc;
make;
./hypercolumn;
python3 plot_spikes_hypercolumn.py output.spikes.csv -noshow
python3 plot_traces.py trace_ampa.csv plot_traces_ampa.png -noshow
python3 plot_traces.py trace_nmda.csv plot_traces_nmda.png -noshow
code -r plot_spikes.png;
code -r plot_traces_ampa.png;
code -r plot_traces_nmda.png;