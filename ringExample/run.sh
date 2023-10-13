#!/bin/bash
cd $(dirname $0)
genn-buildmodel.sh -c tenHHRingModel.cc;
genn-create-user-project.sh tenHHRing tenHHRingSimulation.cc;
make;
./tenHHRing;
python3 ../util/plot_output.py tenHH_output.V.dat;