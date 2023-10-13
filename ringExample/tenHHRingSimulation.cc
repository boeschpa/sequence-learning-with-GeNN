// tenLIFRingModel simulation code
#include "tenHHRing_CODE/definitions.h"
#include <fstream>
#include <iostream>
#include <string>

int main()
{
    allocateMem(); // allocate memory for all neuron variables
    initialize(); // initialize variables and start cpu/gpu kernel

    // init connectivity debug
    pullPop1selfConnectivityFromDevice();
    
    int offset = 0;
    int row = 0;
    int loops = 1;

// try with more neurons, how to know when its done? number of pre neurons!!
// try to build this array.
// check with more empty rows


    // while (indPop1self+offset != 0 && loops < 15){
    //     std::cout << "Row " << row << ": " ;
    //     for (int i = 0; i < *(rowLengthPop1self+offset); i++) {
    //         std::cout << *(indPop1self+i+offset) << ", ";
    //     }
    //     offset += *(rowLengthPop1self+offset);
    //     row++;
    //     loops++;
    //     std::cout << "\n" << "  (offset: " << offset << ")\n";
    // }
    for (int i = 0; i< 30; i++){
        std::cout << *(indPop1self+i) << "\n";
    }
    std::cout << "next value: " << *(indPop1self+offset);
    startSpikeStim[0] = 0;
    endSpikeStim[0] = 1;

    initializeSparse();

    allocatespikeTimesStim(1);
    spikeTimesStim[0] = 0.0f;
    pushspikeTimesStimToDevice(1);

    //t is current simulation time provided by GeNN in ms
    std::ofstream os("tenHH_output.V.dat");
    while (t < 1000.0f) {
        stepTime();
        pullVPop1FromDevice();
        os << t << " ";
        for (int j= 0; j < 10; j++) {
            os << VPop1[j] << " ";
        }
        os << std::endl;
    }
    os.close();


    // pullPop1StateFromDevice(); // gets states of neuron population from gpu, does nothing in cpu mode
    // for (int j= 0; j < 10; j++) {  
    //     std::cout << VPop1[j] << " ";
    //     std::cout << RefracTimePop1[j] << std::endl;
    // }

    return 0;
}

// build simulator makefile "genn-create-user-project.sh tenLIFRing tenLIFRingSimulation.cc"
// build simulator "make"
//run simulator "./tenLIFModel"




// uninitializedParam U
    // // Download GPU-initialized 'b' parameter values from device
    // pullbPInhFromDevice();

    // // Manually initially U of inhibitory population
    // std::transform(&bPInh[0], &bPInh[nInh], &UPInh[0],
    //                [](float b){ return b * -65.0f; });

    // initializeSparse();