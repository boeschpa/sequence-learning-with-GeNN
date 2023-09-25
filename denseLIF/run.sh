#!/bin/bash
cd $(dirname $0)
genn-buildmodel.sh -c denseLIFModel.cc;
genn-create-user-project.sh denseLIF denseLIFSimulation.cc;
make;
./denseLIF;
python3 ../util/plot_output.py denseLIF_output.V.dat;