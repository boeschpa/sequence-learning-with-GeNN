#!/bin/bash
cd $(dirname $0)
genn-buildmodel.sh -c tenLIFRingModel.cc;
genn-create-user-project.sh tenLIFRing tenLIFRingSimulation.cc;
make;
./tenLIFRing;
python3 ../util/plot_output.py tenLIF_output.V.dat;