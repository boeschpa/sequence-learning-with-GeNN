// tenLIFRingModel simulation code
#include "hypercolumn_CODE/definitions.h"
#include "code_interface.h"
#include <fstream>
#include <typeinfo>
#include "model_param.h"
#include "spikeRecorder.h"
#include "spikeArrayRecorder.h"
#include <iostream>


void recordWeights(){
    FILE *weightsAmpa = fopen("weights_ampa.csv", "w");
    FILE *weightsNmda = fopen("weights_nmda.csv", "w");
    for (int i = 0; i < std::end(g_nmda) - std::begin(g_nmda); i++)
    {
        for (int j = 0; j < (int)*maxRowLengths[i]*N_pyramidal; j++)  //(int)*maxRowLengths[i]
        {
            if(j==0){
                fprintf(weightsNmda, "%f",1000.0*(*g_nmda[i])[j]);
                fprintf(weightsAmpa, "%f",1000.0*(*g_ampa[i])[j]);
            }
            else{
                fprintf(weightsNmda, ", %f",1000.0*(*g_nmda[i])[j]);
                fprintf(weightsAmpa, ", %f",1000.0*(*g_ampa[i])[j]);
            }
        }
        fprintf(weightsNmda, "\n");
        fprintf(weightsAmpa, "\n");
    }
    fclose(weightsNmda);
    fclose(weightsAmpa);
}

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
    // weights
    for (int i = 0; i < std::end(wGains) - std::begin(wGains); i++)
    {
        for (int j = 0; j < (int)*maxRowLengths[i] * N_pyramidal; j++)
        {

            (*wGains[i])[j] = gain;
            (*kappas[i])[j] = kappa;
        }
        pushwGains[i](false);
        pushkappas[i](false);
    }
    // bias
    for (int i = 0; i < std::end(biasGains) - std::begin(biasGains); i++)
    {
        for (int j = 0; j < N_pyramidal; j++)
        {
            (*biasGains[i])[j] = gain;
            (*kappasBias[i])[j] = kappa;
        }
        pushbiasGains[i](false);
        pushkappasBias[i](false);
    }
}

int main()
{
    allocateMem(); // allocate memory for all neuron variables
    float sim_time = settle_time+ epochs * N_minicolumns * (pattern_break + pattern_time) + recall_time;
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

    // SETTLE
    setGainAndKappa(0.0, 1.0);
    setAllStimulation(background_freq);
    t_start = t;
    while (t - t_start < settle_time)
    {
        stepTime();
    }


    // TRAINING
    // t is current simulation time provided by GeNN in ms
    setGainAndKappa(0.0, 1.0); // set weight and learning rate - training
    for (int ep = 0; ep < epochs; ep++)
    {
        std::cout << "Training epoch " << ep + 1 << std::endl;

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
    setGainAndKappa(1.0, 0.0);          // set weight and learning rate
    setAllStimulation(background_freq); // set recall frequencies               todo save and load training state
    t_start = t;
    while (t - t_start < recall_time)
    {
        stepTime();
    }

    pullRecordingBuffersFromDevice();
    writeTextSpikeArrayRecording("output.spikes.csv", recordSpkArray, std::end(recordSpkArray) - std::begin(recordSpkArray),
                                 N_pyramidal, int(sim_time / time_step), time_step);
    recordWeights();

    return 0;
}
