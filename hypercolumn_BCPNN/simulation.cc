// tenLIFRingModel simulation code
#include "hypercolumn_CODE/definitions.h"
#include "code_interface.h"
#include <fstream>
#include <typeinfo>
#include "model_param.h"
#include "spikeRecorder.h"
#include "spikeArrayRecorder.h"

void setAllStimulation(float frequency)
{
    float firingProbability = time_step * 1e-3 * frequency;
    for (int i = 0; i < N_pyramidal; i++)
    {
        for (int j = 0; j < std::end(firingProbs) - std::begin(firingProbs); j++)
        {
            (*firingProbs[j])[i] = firingProbability; // dT (ms) / 1000 ms * average spike frequency
        }
    }
    for (int i = 0; i < std::end(pushfiringProbsToDevice) - std::begin(pushfiringProbsToDevice); i++)
    {
        pushfiringProbsToDevice[i](N_pyramidal);
    }
}

void setOnlyBasicStimulation(float frequency, int minicolumn)
{
    for (int i = 0; i < N_pyramidal; i++)
    {
        for (int j = 0; j < std::end(firingProbs) - std::begin(firingProbs); j++)
        {
            int local_minicolumn = j % N_minicolumns; // get local minicolumn number of global index j
            if (local_minicolumn == minicolumn)
            {
                (*firingProbs[j])[i] = time_step * 1e-3 * frequency; // dT (ms) / 1000 ms * average spike frequency
            }
            else
            {
                (*firingProbs[j])[i] = 0.0;
            }
        }
    }
    for (int i = 0; i < std::end(pushfiringProbsToDevice) - std::begin(pushfiringProbsToDevice); i++)
    {
        pushfiringProbsToDevice[i](N_pyramidal);
    }
}

void setGainAndKappa(float gain, float kappa)
{
    for (int i = 0; i < std::end(wGains) - std::begin(wGains); i++)
    {
        (*wGains[i])[i] = gain;
        (*kappas[i])[i] = kappa;
        pushwGains[i](false);
        pushkappas[i](false);
    }
}

int main()
{
    allocateMem(); // allocate memory for all neuron variables
    float sim_time = epochs * N_minicolumns * (pattern_break + pattern_time) + recall_time;
    allocateRecordingBuffers(int(sim_time / time_step));

    initialize(); // initialize variables and start cpu/gpu kernel

    // initialize firingProbs to 0

    for (int i = 0; i < std::end(allocatefiringProbs) - std::begin(allocatefiringProbs); i++)
    {
        allocatefiringProbs[i](N_pyramidal);
    }
    setAllStimulation(0.0);

    initializeSparse();

    // TRAINING
    // t is current simulation time provided by GeNN in ms
    float t_start;
    setGainAndKappa(0.0, 1.0); // set weight and learning rate - training

    for (int ep = 0; ep < epochs; ep++)
    {
        std::cout << "Training epoch " << ep << std::endl;
        for (int mc = 0; mc < N_minicolumns; mc++)
        {
            // set training pattern
            setOnlyBasicStimulation(training_freq, mc);
            t_start = t;
            while (t - t_start < pattern_time)
            {
                stepTime();
            }

            // set training break
            setAllStimulation(0);
            t_start = t;
            while (t - t_start < pattern_break)
            {
                stepTime();
            }
        }
    }

    // RECALL
    setGainAndKappa(1.0, 0.0); // set weight and learning rate
    setAllStimulation(background_freq); // set recall frequencies               todo save and load training state
    t_start = t;
    while (t < recall_time)
    {
        stepTime();
    }

    pullRecordingBuffersFromDevice();
    writeTextSpikeArrayRecording("output.spikes.csv", recordSpkArray, std::end(recordSpkArray) - std::begin(recordSpkArray),
                                 N_pyramidal, int(sim_time / time_step), time_step);

    return 0;
}

// build simulator makefile "genn-create-user-project.sh tenLIFRing tenLIFRingSimulation.cc"
// build simulator "make"
// run simulator "./tenLIFModel"