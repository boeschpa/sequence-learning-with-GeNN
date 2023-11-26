// tenLIFRingModel simulation code
#include "hypercolumn_CODE/definitions.h"
#include "code_interface.h"
#include <fstream>
#include <typeinfo>
#include "model_param.h"
#include "spikeRecorder.h"
#include "spikeArrayRecorder.h"

#define RECORD_TRACE_AMPA fprintf(traceAmpa, "%f, %f, %f\n", t, IwH0_0_M0[0], VH0_0_M0[0])
#define RECORD_TRACE_NMDA fprintf(traceNmda, "%f, %f, %f\n", t, IwH0_0_M0[3], VH0_0_M0[3])
#define RECORD_TRACE RECORD_TRACE_AMPA; RECORD_TRACE_NMDA

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

void setHalfStimulation(float frequency, bool half)
{
    for (int i = 0; i < N_pyramidal; i++)
    {
        for (int j = 0; j < std::end(firingProbs) - std::begin(firingProbs); j++)
        {
            int local_minicolumn = j % N_minicolumns; // get local minicolumn number of global index j
            if ((local_minicolumn >= N_minicolumns / 2) == half)
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
        (*wGains[i])[0] = gain;
        (*kappas[i])[0] = kappa;
        pushwGains[i](false);
        pushkappas[i](false);
    }
}

int main()
{
    allocateMem(); // allocate memory for all neuron variables
    float sim_time = recall_time;
    allocateRecordingBuffers(int(sim_time / time_step));

    initialize(); // initialize variables and start cpu/gpu kernel
    float t_start;

    // initialize firingProbs to 0

    for (int i = 0; i < std::end(allocatefiringProbs) - std::begin(allocatefiringProbs); i++)
    {
        allocatefiringProbs[i](N_pyramidal);
    }
    setAllStimulation(0.0);

    initializeSparse();

    // trace recording
    FILE *traceAmpa = fopen("trace_ampa.csv", "w");
    FILE *traceNmda = fopen("trace_nmda.csv", "w");

    // RECALL
    setGainAndKappa(1.0, 0.0);          // set weight and learning rate
    setAllStimulation(background_freq); // set recall frequencies               todo save and load training state
    t_start = t;
    while (t - t_start < recall_time)
    {
        stepTime();
        pullH0_0_to_H0_0_M0to_0_lateral_ampaStateFromDevice();
        pullH0_0_to_H0_0_M0to_0_lateral_nmdaStateFromDevice();
        pullH0_0_M0StateFromDevice();
        RECORD_TRACE;
    }

    fclose(traceAmpa);
    fclose(traceNmda);
    pullRecordingBuffersFromDevice();
    writeTextSpikeArrayRecording("output.spikes.csv", recordSpkArray, std::end(recordSpkArray) - std::begin(recordSpkArray),
                                 N_pyramidal, int(sim_time / time_step), time_step);

    return 0;
}

// build simulator makefile "genn-create-user-project.sh tenLIFRing tenLIFRingSimulation.cc"
// build simulator "make"
// run simulator "./tenLIFModel"

// todo debug: only one pattern!!
// plot weights ampa 0_0 --> high; 0_5 --> low/negative; etc
//