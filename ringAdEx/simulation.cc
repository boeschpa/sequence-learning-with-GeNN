// tenLIFRingModel simulation code
#include "ringAdEx_CODE/definitions.h"
#include <fstream>

int main()
{
    allocateMem(); // allocate memory for all neuron variables
    initialize(); // initialize variables and start cpu/gpu kernel

    startSpikeStim[0] = 0;
    endSpikeStim[0] = 1;

    initializeSparse();

    allocatespikeTimesStim(1);
    spikeTimesStim[0] = 0.0f;
    pushspikeTimesStimToDevice(1);

    //t is current simulation time provided by GeNN in ms
    std::ofstream os("output.V.dat");
    while (t < 100.0f) {
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