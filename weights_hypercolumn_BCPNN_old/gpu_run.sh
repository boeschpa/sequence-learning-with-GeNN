#!/bin/bash
cd $(dirname $0)
# rm -r hypercolumn_CODE
# rm generator
# rm generator.d
# rm hypercolumn
# rm Makefile
# rm output.spikes.csv
# rm code_interface.h

genn-buildmodel.sh -f model.cc;
python3 generate_code_interface.py
genn-create-user-project.sh hypercolumn simulation.cc;
echo ">>> start compilation";
SECONDS=0;
make;
echo ">>> compile time: ${SECONDS}s";
echo ">>> start simulation";
SECONDS=0;
./hypercolumn;
echo ">>> simulation time: ${SECONDS}s";
python3 plot_spikes_hypercolumn.py output.spikes.csv -noshow
#python3 plot_traces.py trace_ampa.csv plot_traces_ampa.png -noshow
#python3 plot_traces.py trace_nmda.csv plot_traces_nmda.png -noshow
#code -r plot_spikes.png;
#code -r plot_traces_ampa.png;
#code -r plot_traces_nmda.png;
