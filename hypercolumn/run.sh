#!/bin/bash
cd $(dirname $0)
genn-buildmodel.sh -c model.cc;
genn-create-user-project.sh ringAdEx simulation.cc;
make;
./ringAdEx;
#python3 ../util/plot_output.py output.V.dat;