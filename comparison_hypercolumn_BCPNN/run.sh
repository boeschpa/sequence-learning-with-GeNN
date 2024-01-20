#!/bin/bash
cd $(dirname $0)

genn-buildmodel.sh -cf model.cc; # build model, generates CPU code in *_CODE folder (GeNN)
python3 generate_code_interface.py # generate custom variable and function pointer interface (allows parametrized model architecture)
genn-create-user-project.sh hypercolumn simulation.cc; # create simulator makefile
echo ">>> start compilation";
SECONDS=0; # measure time
make; # compile
echo ">>> compile time: ${SECONDS}s"; # output compile time
echo ">>> start simulation";
SECONDS=0; # measure time
./hypercolumn; # run
echo ">>> simulation time: ${SECONDS}s"; # output simulation time
python3 plot_spikes.py