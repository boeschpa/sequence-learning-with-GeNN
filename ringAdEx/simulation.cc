// tenLIFRingModel simulation code
#include "ringAdEx_CODE/definitions.h"
#include <fstream>

int main()
{
    int N = 20;
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
    std::ofstream os_spike("output.spikes.dat");
    while (t < 1000.0f) {
        stepTime();
        pullVPop1FromDevice(); //get voltages
        pullPop1SpikesFromDevice(); // get spikes
        pullPop1CurrentSpikesFromDevice();
        //std::cout << "spkQuePtrPop1: " << spkQuePtrPop1 << "\n";
        //std::cout << "glbSpkCntPop1: " << glbSpkCntPop1[0] << "\n"; 
        os << t << " ";
        os_spike << t << " ";
        for (int j= 0; j < N; j++) {
            os << VPop1[j] << " ";
            
        }
        os_spike << spikeCount_Pop1 << " ";
        os << std::endl;
        os_spike << std::endl;
    }
    os.close();
    os_spike.close();


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