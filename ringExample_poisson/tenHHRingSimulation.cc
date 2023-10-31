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

    allocatefiringProbStim(2); // argument 0: count
    firingProbStim[0] = 1.0 / 100.0;
    firingProbStim[1] = 1.0 / 100.0;
    pushfiringProbStimToDevice(2); // argument 0: count

    initializeSparse();

    // allocatespikeTimesStim(1);
    // spikeTimesStim[0] = 0.0f;
    // spikeTimesStim[1] = 100.0f;
    // spikeTimesStim[2] = 200.0f;
    // pushspikeTimesStimToDevice(1);

    // t is current simulation time provided by GeNN in ms
    std::ofstream os("tenHH_output.V.dat");
    std::ofstream os_spike("tenHH_output.spikes.dat");
    bool switched1 = false;
    while (t < 1000.0f)
    {
        if (t > 500.0f && !switched1)
        {
            firingProbStim[0] = 0.0;
            firingProbStim[1] = 1.0/1000.0;
            pushfiringProbStimToDevice(2); // argument 0: count
            switched1 = true;
        }

        stepTime();
        pullVPop1FromDevice();      // get voltages
        pullPop1SpikesFromDevice(); // get spikes
        pullPop1CurrentSpikesFromDevice();
        // std::cout << "spkQuePtrPop1: " << spkQuePtrPop1 << "\n";
        // std::cout << "glbSpkCntPop1: " << glbSpkCntPop1[0] << "\n";
        os << t << " ";
        os_spike << t << " ";
        for (int j = 0; j < 10; j++)
        {
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
// run simulator "./tenLIFModel"

// uninitializedParam U
// // Download GPU-initialized 'b' parameter values from device
// pullbPInhFromDevice();

// // Manually initially U of inhibitory population
// std::transform(&bPInh[0], &bPInh[nInh], &UPInh[0],
//                [](float b){ return b * -65.0f; });

// initializeSparse();