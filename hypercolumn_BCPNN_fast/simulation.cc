// tenLIFRingModel simulation code
#include "hypercolumn_CODE/definitions.h"
#include "code_interface.h"
#include <fstream>
#include <typeinfo>
#include "model_param.h"
#include "spikeRecorder.h"
#include "spikeArrayRecorder.h"
#include <iostream>
#include <cstdlib>
#include <random>
#include <algorithm>

// #define TRACES

#define RECORD_TRACE_AMPA fprintf(traceAmpa, "%f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f\n", t, 1000.0 * gH0_0_to_H0_0_lateral_ampa[1], 1000.0 * gH0_1_to_H0_0_lateral_ampa[1], 1000.0 * gH0_0_to_H0_0_lateral_ampa[10], 1000.0 * gH0_1_to_H0_0_lateral_ampa[maxRowLengthH0_0_to_H0_0_lateral_ampa], 1000.0 * gH0_0_to_H0_0_lateral_ampa[maxRowLengthH0_0_to_H0_0_lateral_ampa], 1000.0 * gH0_1_to_H0_0_lateral_ampa[maxRowLengthH0_0_to_H0_0_lateral_ampa], PiH0_0_to_H0_0_lateral_ampa[1], PjH0_0_to_H0_0_lateral_ampa[1], PijH0_0_to_H0_0_lateral_ampa[1], ZiH0_0_to_H0_0_lateral_ampa[1], ZjH0_0_to_H0_0_lateral_ampa[1])
#define RECORD_TRACE_NMDA fprintf(traceNmda, "%f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f\n", t, 1000.0 * gH0_0_to_H0_0_lateral_nmda[1], 1000.0 * gH0_1_to_H0_0_lateral_nmda[1], 1000.0 * gH0_0_to_H0_0_lateral_nmda[10], 1000.0 * gH0_1_to_H0_0_lateral_nmda[maxRowLengthH0_0_to_H0_0_lateral_nmda], 1000.0 * gH0_0_to_H0_0_lateral_nmda[maxRowLengthH0_0_to_H0_0_lateral_nmda], 1000.0 * gH0_1_to_H0_0_lateral_nmda[maxRowLengthH0_0_to_H0_0_lateral_nmda], PiH0_0_to_H0_0_lateral_nmda[1], PjH0_0_to_H0_0_lateral_nmda[1], PijH0_0_to_H0_0_lateral_nmda[1], ZiH0_0_to_H0_0_lateral_nmda[1], ZjH0_0_to_H0_0_lateral_nmda[1])
#define RECORD_TRACE   \
    RECORD_TRACE_AMPA; \
    RECORD_TRACE_NMDA

void recordVmem(FILE *traceVmem, scalar *neuron_pop)
{
    fprintf(traceVmem, "%f", t);
    for (int i = 0; i < N_pyramidal * N_minicolumns; i++)
    {
        fprintf(traceVmem, ", %f", neuron_pop[i]);
    }
    fprintf(traceVmem, "\n");
}

void recordWeights()
{
    FILE *weightsAmpa = fopen("weights_ampa.csv", "w");
    FILE *weightsNmda = fopen("weights_nmda.csv", "w");
    for (int i = 0; i < std::end(g_nmda) - std::begin(g_nmda); i++)
    {
        for (int j = 0; j < N_minicolumns * N_pyramidal * N_minicolumns * N_pyramidal; j++)
        {
            if (j == 0)
            {
                fprintf(weightsNmda, "%f", 1000.0 * (*g_nmda[i])[j]);
                fprintf(weightsAmpa, "%f", 1000.0 * (*g_ampa[i])[j]);
            }
            else
            {
                fprintf(weightsNmda, ", %f", 1000.0 * (*g_nmda[i])[j]);
                fprintf(weightsAmpa, ", %f", 1000.0 * (*g_ampa[i])[j]);
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
    for (int i = 0; i < N_minicolumns * N_pyramidal; i++)
    {
        for (int j = 0; j < std::end(firingProbs) - std::begin(firingProbs); j++)
        {

            (*firingProbs[j])[i] = firingProbability; // dT (ms) / 1000 ms * average spike frequency
        }
    }
    for (int i = 0; i < std::end(pushfiringProbsToDevice) - std::begin(pushfiringProbsToDevice); i++)
    {
        pushfiringProbsToDevice[i](N_minicolumns * N_pyramidal);
    }
}

// Function to generate a random pattern with a specified seed
int *generateRandomPattern(int N_hypercolumns, int N_minicolumns, unsigned int seed)
{
    // Use the provided seed to seed the random number generator
    std::mt19937 gen(seed);

    // Create a distribution to generate random integers
    std::uniform_int_distribution<int> distribution(0, N_minicolumns - 1);

    int *random_pattern = new int[N_hypercolumns];

    // Fill the array with random minicolumn indices
    for (int i = 0; i < N_hypercolumns; ++i)
    {
        random_pattern[i] = distribution(gen);
    }

    return random_pattern;
}

// Function to generate a trivial diagonal pattern
int *generateDiagonalPattern(int N_hypercolumns, int minicolumn)
{
    int *pattern = new int[N_hypercolumns];

    // Fill the array with random minicolumn indices
    for (int i = 0; i < N_hypercolumns; ++i)
    {
        pattern[i] = minicolumn;
    }

    return pattern;
}

int **generateRandomSequence(int N_patterns, int N_hypercolumns, int N_minicolumns, unsigned int seed)
{
    int **randomSequence = new int *[N_patterns];
    for (int i = 0; i < N_patterns; i++)
    {
        randomSequence[i] = generateRandomPattern(N_hypercolumns, N_minicolumns, seed + i);
    }
    return randomSequence;
}

int **generateDiagonalSequence(int N_patterns, int N_hypercolumns)
{
    int **sequence = new int *[N_patterns];
    for (int i = 0; i < N_patterns; i++)
    {
        sequence[i] = generateDiagonalPattern(N_hypercolumns, i);
    }
    return sequence;
}

void saveSequence(int **sequence, int N_patterns, int N_hypercolumns)
{
    FILE *sequenceFile = fopen("sequence.csv", "w");
    for (int pat = 0; pat < N_patterns; pat++)
    {
        int *pattern = sequence[pat];
        for (int mc = 0; mc < N_hypercolumns; mc++)
        {
            if (mc == 0)
            {
                fprintf(sequenceFile, "%i", pattern[mc]);
            }
            else
            {
                fprintf(sequenceFile, ", %i", pattern[mc]);
            }
        }
        fprintf(sequenceFile, "\n");
    }
    fclose(sequenceFile);
}

void setPattern(float frequency, int *pattern)
{
    for (int i = 0; i < std::end(firingProbs) - std::begin(firingProbs); i++)
    {
        int active = pattern[i]; // active MC in current hypercolumn i
        int mc_index;
        for (int j = 0; j < N_minicolumns * N_pyramidal; j++)
        {
            mc_index = j / N_pyramidal; // minicolumn index of neuron index j
            if (mc_index == active)
            {
                (*firingProbs[i])[j] = time_step * 1e-3 * frequency; // dT (ms) / 1000 ms * average spike frequency
            }
            else
            {
                (*firingProbs[i])[j] = 0.0;
            }
        }
    }
    for (int i = 0; i < std::end(pushfiringProbsToDevice) - std::begin(pushfiringProbsToDevice); i++)
    {
        pushfiringProbsToDevice[i](N_minicolumns * N_pyramidal);
    }
}

void setGainAndKappa(float gain, float kappa)
{
    // weights
    for (int i = 0; i < std::end(wGains) - std::begin(wGains); i++)
    {
        for (int j = 0; j < (int)*maxRowLengths[i] * N_pyramidal * N_minicolumns; j++)
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
        for (int j = 0; j < N_pyramidal * N_minicolumns; j++)
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
    float buffer_time = epochs * N_patterns * (pattern_break + pattern_time) + recall_break + recall_time;
    // float buffer_time = std::max(std::max( N_patterns * (pattern_break + pattern_time), recall_break), recall_time);
    allocateRecordingBuffers(int(buffer_time / time_step));

    initialize(); // initialize variables and start cpu/gpu kernel
    float t_start;

    // initialize firingProbs to 0

    for (int i = 0; i < std::end(allocatefiringProbs) - std::begin(allocatefiringProbs); i++)
    {
        allocatefiringProbs[i](N_minicolumns * N_pyramidal);
    }
    setAllStimulation(0.0);

    // generate random sequence
    int **sequence = generateRandomSequence(N_patterns, hyper_width * hyper_height, N_minicolumns, 42);
    // int **sequence = generateDiagonalSequence(N_patterns, hyper_width * hyper_height);
    saveSequence(sequence, N_patterns, hyper_width * hyper_height);

    initializeSparse();

// trace recording
#ifdef TRACES
    FILE *traceAmpa = fopen("trace_ampa.csv", "w");
    FILE *traceNmda = fopen("trace_nmda.csv", "w");
#endif

    // vmem recording
    FILE *traceVmem = fopen("trace_vmem.csv", "w");

    // TRAINING
    // t is current simulation time provided by GeNN in ms
    setGainAndKappa(0.0, 1.0); // set weight and learning rate - training
    for (int ep = 0; ep < epochs; ep++)
    {
        std::cout << "Training epoch " << ep + 1 << std::endl;

        for (int pat = 0; pat < N_patterns; pat++)
        {
            int *pattern = sequence[pat];

            // set training pattern
            setPattern(training_freq, pattern);
            t_start = t;
            while (t - t_start < pattern_time)
            {
                stepTime();
#ifdef TRACES
                pullH0_0_to_H0_0_lateral_ampaStateFromDevice();
                pullH0_0_to_H0_0_lateral_nmdaStateFromDevice();
                RECORD_TRACE;
#endif
                pullH0_0StateFromDevice();
                recordVmem(traceVmem, VH0_0);
            }

            // set training break
            setAllStimulation(0);
            t_start = t;
            while (t - t_start < pattern_break)
            {
                stepTime();
#ifdef TRACES
                pullH0_0_to_H0_0_lateral_ampaStateFromDevice();
                pullH0_0_to_H0_0_lateral_nmdaStateFromDevice();
                RECORD_TRACE;
#endif
                pullH0_0StateFromDevice();
                recordVmem(traceVmem, VH0_0);
            }
        }
        // writeTextSpikeArrayRecording("output.spikes.csv", recordSpkArray, std::end(recordSpkArray) - std::begin(recordSpkArray),
        //                          N_minicolumns * N_pyramidal, int(buffer_time / time_step), time_step);
    }

    // RECALL BREAK
    setGainAndKappa(1.0, 0.0);
    setAllStimulation(0.0);
    t_start = t;
    while (t - t_start < recall_break)
    {
        stepTime();
#ifdef TRACES
        pullH0_0_to_H0_0_lateral_ampaStateFromDevice();
        pullH0_0_to_H0_0_lateral_nmdaStateFromDevice();
        RECORD_TRACE;
#endif
        pullH0_0StateFromDevice();
        recordVmem(traceVmem, VH0_0);
    }
    // writeTextSpikeArrayRecording("output.spikes.csv", recordSpkArray, std::end(recordSpkArray) - std::begin(recordSpkArray),
    //                              N_minicolumns * N_pyramidal, int(buffer_time / time_step), time_step, " ", false, true);

    // RECALL
    setGainAndKappa(1.0, 0.0);          // set weight and learning rate
    setAllStimulation(background_freq); // set recall frequencies               todo save and load training state
    t_start = t;
    while (t - t_start < recall_time)
    {
        stepTime();
#ifdef TRACES
        pullH0_0_to_H0_0_lateral_ampaStateFromDevice();
        pullH0_0_to_H0_0_lateral_nmdaStateFromDevice();
        RECORD_TRACE;
#endif
        pullH0_0StateFromDevice();
        recordVmem(traceVmem, VH0_0);
    }
#ifdef TRACES
    fclose(traceAmpa);
    fclose(traceNmda);
#endif
    fclose(traceVmem);
    pullRecordingBuffersFromDevice();
    writeTextSpikeArrayRecording("output.spikes.csv", recordSpkArray, std::end(recordSpkArray) - std::begin(recordSpkArray),
                                 N_minicolumns * N_pyramidal, int(buffer_time / time_step), time_step, " ", false, false);
    // recordWeights();

    return 0;
}