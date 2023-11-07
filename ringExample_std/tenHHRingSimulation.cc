// tenLIFRingModel simulation code
#include "tenHHRing_CODE/definitions.h"
#include <fstream>
#include <iostream>
#include <string>
// #include "spikeRecorder.h"

int main()
{
    allocateMem(); // allocate memory for all neuron variables
    initialize();  // initialize variables and start cpu/gpu kernel

    int n_stim = 20;
    float period = 20.0;
    float break_time = 200.0;

    startSpikeStim[0] = 0;
    endSpikeStim[0] = n_stim;

    initializeSparse();

    allocatespikeTimesStim(n_stim);
    for (int i = 0; i < n_stim; i++)
    {
        if (i < n_stim / 2)
        {
            spikeTimesStim[i] = period * i;
        }
        else
        {
            spikeTimesStim[i] = break_time + period * i;
        }
    }

    pushspikeTimesStimToDevice(n_stim);

    // t is current simulation time provided by GeNN in ms
    std::ofstream os("tenHH_output.V.dat");
    std::ofstream os_spike("tenHH_output.spikes.dat");
    while (t < 1000.0f)
    {
        stepTime();
        pullVPop1FromDevice();      // get voltages
        pullPop1SpikesFromDevice(); // get spikes
        pullxStimPop1FromDevice(); // get x std variable
        pullPop1CurrentSpikesFromDevice();
        // std::cout << "spkQuePtrPop1: " << spkQuePtrPop1 << "\n";
        // std::cout << "glbSpkCntPop1: " << glbSpkCntPop1[0] << "\n";
        os << t << " ";
        os_spike << t << " ";
        for (int j = 0; j < 1; j++)
        {
            os << VPop1[j] << " ";
            os << xStimPop1[j]*100.0 << " ";
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
// run simulator "./tenLIFModel"

// uninitializedParam U
// // Download GPU-initialized 'b' parameter values from device
// pullbPInhFromDevice();

// // Manually initially U of inhibitory population
// std::transform(&bPInh[0], &bPInh[nInh], &UPInh[0],
//                [](float b){ return b * -65.0f; });

// initializeSparse();