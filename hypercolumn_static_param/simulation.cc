// tenLIFRingModel simulation code
#include "hypercolumn_CODE/definitions.h"
#include "code_interface.h"
#include <fstream>
#include <typeinfo>
#include "model_param.h"
#include "spikeRecorder.h"
#include"spikeArrayRecorder.h"

int main()
{
    allocateMem(); // allocate memory for all neuron variables
    allocateRecordingBuffers(int(sim_time/time_step));

    initialize();  // initialize variables and start cpu/gpu kernel

    int N_pyramidal = 10; // usually = N_pyramidal
    float background_freq = 200.0;

    // for (int i = 0; i < 4; i++)
    // {
    //     std::cout << (spikeCounts[i]) << std::endl;
    // }

    for (int i = 0; i < std::end(allocatefiringProbs) - std::begin(allocatefiringProbs); i++)
    {
        allocatefiringProbs[i](N_pyramidal);
    }

    for (int i = 0; i < N_pyramidal; i++)
    {
        //firingProbH0_0_M0_input[i] = 0.1 * 1e-3 * stim_freq; // dT (ms) / 1000 ms * average spike frequency
        for (int j = 0; j < std::end(firingProbs) - std::begin(firingProbs); j++)
        {
            (*firingProbs[j])[i] = 0.1 * 1e-3 * background_freq;
        }
    }

    for (int i = 0; i < std::end(pushfiringProbsToDevice) - std::begin(pushfiringProbsToDevice); i++)
    {
        pushfiringProbsToDevice[i](N_pyramidal);
    }

    initializeSparse();

    // t is current simulation time provided by GeNN in ms
    while (t < sim_time)
    {
        stepTime();
        // get spikes

    }

    pullRecordingBuffersFromDevice();
    writeTextSpikeArrayRecording("output.spikes.csv", recordSpkArray, std::end(recordSpkArray) - std::begin(recordSpkArray),
                            N_pyramidal, int(sim_time/time_step), time_step);

    return 0;
}

// build simulator makefile "genn-create-user-project.sh tenLIFRing tenLIFRingSimulation.cc"
// build simulator "make"
// run simulator "./tenLIFModel"