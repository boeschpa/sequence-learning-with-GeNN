// tenLIFRingModel simulation code
#include "example_CODE/definitions.h"
#include <fstream>
#include <iostream>
#include <string>
//#include "spikeRecorder.h"

int main()
{
    allocateMem(); // allocate memory for all neuron variables
    initialize(); // initialize variables and start cpu/gpu kernel

    startSpikeStim[0] = 0;
    endSpikeStim[0] = 3;

    initializeSparse();

    allocatespikeTimesStim(1);
    spikeTimesStim[0] = 0.0f;
    spikeTimesStim[1] = 233.0f;
    spikeTimesStim[2] = 466.0f;
    pushspikeTimesStimToDevice(1);

    //t is current simulation time provided by GeNN in ms
    std::ofstream os("output.V.dat");
    std::ofstream os_spike("output.spikes.dat");
    while (t < 1000.0f) {
        stepTime();
        pullVPop1FromDevice(); //get voltages
        pullPop1SpikesFromDevice(); // get spikes
        pullPop1CurrentSpikesFromDevice();

        os << t << " ";
        os_spike << t << " ";
        for (int j= 0; j < 1; j++) {
            os << VPop1[j] << " ";
            
        }
        os_spike << spikeCount_Pop1 << " ";
        os << std::endl;
        os_spike << std::endl;
    }
    os.close();
    os_spike.close();

    return 0;
}