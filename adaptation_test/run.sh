#!/bin/bash
cd $(dirname $0)
genn-buildmodel.sh -c model.cc;
genn-create-user-project.sh test simulation.cc;
make;
./test;
python3 plot_example.py -noshow;
code -r plot_example.pdf;