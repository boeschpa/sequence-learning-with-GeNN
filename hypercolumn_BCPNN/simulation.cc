// tenLIFRingModel simulation code
#include "hypercolumn_CODE/definitions.h"
#include "code_interface.h"
#include <fstream>
#include <typeinfo>
#include "model_param.h"
#include "spikeRecorder.h"
#include "spikeArrayRecorder.h"

#define RECORD_TRACE_AMPA fprintf(traceAmpa, "%f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f\n", t, 1000.0 * gH0_0_to_H0_0_M0to_0_lateral_ampa[1], 1000.0 * gH0_1_to_H0_0_M0to_0_lateral_ampa[1], 1000.0 * gH0_0_to_H0_0_M0to_1_lateral_ampa[1], 1000.0 * gH0_1_to_H0_0_M0to_1_lateral_ampa[1], 1000.0 * gH0_0_to_H0_0_M1to_0_lateral_ampa[1], 1000.0 * gH0_1_to_H0_0_M1to_0_lateral_ampa[1], PiH0_0_to_H0_0_M0to_0_lateral_ampa[1], PjH0_0_to_H0_0_M0to_0_lateral_ampa[1], PijH0_0_to_H0_0_M0to_0_lateral_ampa[1], ZiH0_0_to_H0_0_M0to_0_lateral_ampa[1], ZjH0_0_to_H0_0_M0to_0_lateral_ampa[1])
#define RECORD_TRACE_NMDA fprintf(traceNmda, "%f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f\n", t, 1000.0 * gH0_0_to_H0_0_M0to_0_lateral_nmda[1], 1000.0 * gH0_1_to_H0_0_M0to_0_lateral_nmda[1], 1000.0 * gH0_0_to_H0_0_M0to_1_lateral_nmda[1], 1000.0 * gH0_1_to_H0_0_M0to_1_lateral_nmda[1], 1000.0 * gH0_0_to_H0_0_M1to_0_lateral_nmda[1], 1000.0 * gH0_1_to_H0_0_M1to_0_lateral_nmda[1], PiH0_0_to_H0_0_M0to_0_lateral_nmda[1], PjH0_0_to_H0_0_M0to_0_lateral_nmda[1], PijH0_0_to_H0_0_M0to_0_lateral_nmda[1], ZiH0_0_to_H0_0_M0to_0_lateral_nmda[1], ZjH0_0_to_H0_0_M0to_0_lateral_nmda[1])
#define RECORD_TRACE   \
    RECORD_TRACE_AMPA; \
    RECORD_TRACE_NMDA


void recordWeights(){
    FILE *weightsAmpa = fopen("weights_ampa.csv", "w");
    FILE *weightsNmda = fopen("weights_nmda.csv", "w");
    for (int i = 0; i < std::end(g_nmda) - std::begin(g_nmda); i++)
    {
        for (int j = 0; j < N_pyramidal*N_pyramidal; j++)
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
    for (int i = 0; i < std::end(wGains) - std::begin(wGains); i++)
    {
        for (int j = 0; j < N_pyramidal*N_pyramidal; j++)
        {

            (*wGains[i])[j] = gain;
            (*kappas[i])[j] = kappa;
        }
        pushwGains[i](false);
        pushkappas[i](false);
    }
}

int main()
{
    allocateMem(); // allocate memory for all neuron variables
    float sim_time = epochs * N_minicolumns * (pattern_break + pattern_time) + recall_break + recall_time;
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

    // // RECALL before training
    // setGainAndKappa(1.0, 0.0);          // set weight and learning rate
    // setAllStimulation(background_freq); // set recall frequencies               todo save and load training state
    // t_start = t;
    // while (t - t_start < recall_time)
    // {
    //     stepTime();
    // }

    // // RECALL BREAK
    // setAllStimulation(0.0);
    // t_start = t;
    // while (t - t_start < recall_break)
    // {
    //     stepTime();
    // }

    // TRAINING
    // t is current simulation time provided by GeNN in ms
    setGainAndKappa(0.0, 1.0); // set weight and learning rate - training
    for (int ep = 0; ep < epochs; ep++)
    {
        std::cout << "Training epoch " << ep + 1 << std::endl;

        // setHalfStimulation(training_freq, 1);
        // t_start = t;
        // while (t - t_start < pattern_time)
        // {
        //     stepTime();
        // }

        // t_start = t;
        // while (t - t_start < pattern_break)
        // {
        //     stepTime();
        // }

        // setHalfStimulation(training_freq, 0);
        // t_start = t;
        // while (t - t_start < pattern_time)
        // {
        //     stepTime();
        // }

        // t_start = t;
        // while (t - t_start < pattern_break)
        // {
        //     stepTime();
        // }

        for (int mc = 0; mc < N_minicolumns; mc++)
        {
            // set training pattern
            //setOnlyBasicStimulation(training_freq, mc);
            setAllStimulation(training_freq);
            t_start = t;
            while (t - t_start < pattern_time)
            {
                stepTime();
                pullH0_0_to_H0_0_M0to_0_lateral_ampaStateFromDevice();
                pullH0_0_to_H0_0_M0to_0_lateral_nmdaStateFromDevice();
                RECORD_TRACE;
            }

            // set training break
            setAllStimulation(0);
            t_start = t;
            while (t - t_start < pattern_break)
            {
                stepTime();
                pullH0_0_to_H0_0_M0to_0_lateral_ampaStateFromDevice();
                pullH0_0_to_H0_0_M0to_0_lateral_nmdaStateFromDevice();
                RECORD_TRACE;
            }
        }
    }

    // RECALL BREAK
    setGainAndKappa(1.0, 0.0);
    setAllStimulation(0.0);
    t_start = t;
    while (t - t_start < recall_break)
    {
        stepTime();
        pullH0_0_to_H0_0_M0to_0_lateral_ampaStateFromDevice();
        pullH0_0_to_H0_0_M0to_0_lateral_nmdaStateFromDevice();
        RECORD_TRACE;
    }

    // RECALL
    setGainAndKappa(1.0, 0.0);          // set weight and learning rate
    setAllStimulation(background_freq); // set recall frequencies               todo save and load training state
    t_start = t;
    while (t - t_start < recall_time)
    {
        stepTime();
        pullH0_0_to_H0_0_M0to_0_lateral_ampaStateFromDevice();
        pullH0_0_to_H0_0_M0to_0_lateral_nmdaStateFromDevice();
        RECORD_TRACE;
    }

    fclose(traceAmpa);
    fclose(traceNmda);
    pullRecordingBuffersFromDevice();
    writeTextSpikeArrayRecording("output.spikes.csv", recordSpkArray, std::end(recordSpkArray) - std::begin(recordSpkArray),
                                 N_pyramidal, int(sim_time / time_step), time_step);
    recordWeights();

    return 0;
}

// build simulator makefile "genn-create-user-project.sh tenLIFRing tenLIFRingSimulation.cc"
// build simulator "make"
// run simulator "./tenLIFModel"

// todo debug: only one pattern!!
// plot weights ampa 0_0 --> high; 0_5 --> low/negative; etc
//