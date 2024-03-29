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
#include <iostream>
#include <vector>

//#define TRACES

//#define RECORD_TRACE_AMPA fprintf(traceAmpa, "%f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f\n", t, 1000.0 * gH0_0_to_H0_0_lateral_ampa[1], 1000.0 * gH0_1_to_H0_0_lateral_ampa[1], 1000.0 * gH0_0_to_H0_0_lateral_ampa[10], 1000.0 * gH0_1_to_H0_0_lateral_ampa[maxRowLengthH0_0_to_H0_0_lateral_ampa], 1000.0 * gH0_0_to_H0_0_lateral_ampa[maxRowLengthH0_0_to_H0_0_lateral_ampa], 1000.0 * gH0_1_to_H0_0_lateral_ampa[maxRowLengthH0_0_to_H0_0_lateral_ampa], PiH0_0_to_H0_0_lateral_ampa[1], PjH0_0_to_H0_0_lateral_ampa[1], PijH0_0_to_H0_0_lateral_ampa[1], ZiH0_0_to_H0_0_lateral_ampa[1], ZjH0_0_to_H0_0_lateral_ampa[1])
#define RECORD_TRACE_AMPA fprintf(traceAmpa, "%f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f\n", t, 1000.0 * IbH0_0[0], 1000.0 * IbH0_0[1], 1000.0 * IbH0_0[2], 1000.0 * IbH0_0[3], 1000.0 * IbH0_0[4], 1000.0 * IbH0_0[5], PjH0_0[0], PjH0_0[1], PjH0_0[2], ZjH0_0[0], ZjH0_0[1])
#define RECORD_TRACE_NMDA fprintf(traceNmda, "%f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f\n", t, 1000.0 * gH0_0_to_H0_0_lateral_nmda[1], 1000.0 * gH0_1_to_H0_0_lateral_nmda[1], 1000.0 * gH0_0_to_H0_0_lateral_nmda[10], 1000.0 * gH0_1_to_H0_0_lateral_nmda[maxRowLengthH0_0_to_H0_0_lateral_nmda], 1000.0 * gH0_0_to_H0_0_lateral_nmda[maxRowLengthH0_0_to_H0_0_lateral_nmda], 1000.0 * gH0_1_to_H0_0_lateral_nmda[maxRowLengthH0_0_to_H0_0_lateral_nmda], PiH0_0_to_H0_0_lateral_nmda[1], PjH0_0_to_H0_0_lateral_nmda[1], PijH0_0_to_H0_0_lateral_nmda[1], ZiH0_0_to_H0_0_lateral_nmda[1], ZjH0_0_to_H0_0_lateral_nmda[1])
#define RECORD_TRACE   \
    RECORD_TRACE_AMPA; \
    RECORD_TRACE_NMDA

#define VMEM

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
    for (int i = 0; i < std::end(pullStates) - std::begin(pullStates); i++){
        pullStates[i]();
    }

    // matrix of all minicolumn combinations containing a vector for storing all corresponding weights
    std::vector<std::vector<std::vector<float>>> weightsAmpa(N_minicolumns, std::vector<std::vector<float>>(N_minicolumns));
    std::vector<std::vector<std::vector<float>>> weightsNmda(N_minicolumns, std::vector<std::vector<float>>(N_minicolumns));

    for (int i = 0; i < std::end(g_nmda) - std::begin(g_nmda); i++) // iterate through HC connections
    {
        int maxRowAmpa = *maxRowLengthAmpas[i];
        for (int j = 0; j <  N_minicolumns*N_pyramidal; j++)  // iterate through minicolumns and neurons (rows)
        {
            int rowlen = (*rowLengthAmpas[i])[j];
            for (int k = 0; k< rowlen; k++){ // iterate inside row
                int id = k + maxRowAmpa*j; // k + offset
                float g = 1000.0 * (*g_ampa[i])[id];
                int ind = (*indAmpas[i])[id];

                int preId = j/N_pyramidal; // presynaptic (row) minicolumn index
                int postId = ind/N_pyramidal; // postsynaptic/target minicolumn index
                
                // add weight to matrix
                weightsAmpa[preId][postId].push_back(g);
            }
        }

        int maxRowNmda = *maxRowLengthNmdas[i];
        for (int j = 0; j <  N_minicolumns*N_pyramidal; j++)  // iterate through minicolumns and neurons (rows)
        {
            int rowlen = (*rowLengthNmdas[i])[j];
            for (int k = 0; k< rowlen; k++){ // iterate inside row
                int id = k + maxRowNmda*j; // k + offset
                float g = 1000.0 * (*g_nmda[i])[id];
                int ind = (*indNmdas[i])[id];

                int preId = j/N_pyramidal; // presynaptic (row) minicolumn index
                int postId = ind/N_pyramidal; // postsynaptic/target minicolumn index
                
                // add weight to matrix
                weightsNmda[preId][postId].push_back(g);
            }
        }

    }

    // save data to csv
    FILE *fileWeightsAmpa = fopen("weights_ampa.csv", "w");
    FILE *fileWeightsNmda = fopen("weights_nmda.csv", "w");

    for (int i = 0; i < N_minicolumns; ++i) {
        for (int j = 0; j < N_minicolumns; ++j) {
            for (int k = 0; k < weightsAmpa[i][j].size(); ++k) {
                if (k==0) fprintf(fileWeightsAmpa, "%f", weightsAmpa[i][j][k]);
                else fprintf(fileWeightsAmpa, ", %f", weightsAmpa[i][j][k]);
            }
            fprintf(fileWeightsAmpa, "\n");
        }
    }
    for (int i = 0; i < N_minicolumns; ++i) {
        for (int j = 0; j < N_minicolumns; ++j) {
            for (int k = 0; k < weightsNmda[i][j].size(); ++k) {
                if (k==0) fprintf(fileWeightsNmda, "%f", weightsNmda[i][j][k]);
                else fprintf(fileWeightsNmda, ", %f", weightsNmda[i][j][k]);
            }
            fprintf(fileWeightsNmda, "\n");
        }
    }
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

void saveSequence(int **sequence, int N_patterns, int N_hypercolumns, std::string file_name = "sequence.csv")
{
    FILE *sequenceFile = fopen(file_name.c_str(), "w");
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
        int maxRow;
        if (i%2==0){
            maxRow = (int)*maxRowLengthNmdas[i/2];
        }
        else{
            maxRow = (int)*maxRowLengthAmpas[i/2];
        }
        for (int j = 0; j <  maxRow * N_pyramidal * N_minicolumns; j++)
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

// takes arguments: (0) seed, (1) sequence_length
int main(int argc, char **argv)
{
    allocateMem(); // allocate memory for all neuron variables
    int seed = 0;
    int sequence_length = N_patterns;
    std::string output_name = "";

    // process arguments
    if (argc > 1)
    {
        std::string seed_string = argv[1];
        seed = std::stoi(seed_string);
        output_name = "_seed" + std::to_string(seed);
    }
    if (argc > 2)
    {
        std::string sequence_length_string = argv[2];
        sequence_length = std::stoi(sequence_length_string);
        output_name += "_len" + std::to_string(sequence_length);
    }

    // allocate recording buffer
    float buffer_time = settle_time + epochs * sequence_length * (pattern_break + pattern_time) + recall_time;
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
    //int **sequence = generateRandomSequence(sequence_length, hyper_width * hyper_height, N_minicolumns, seed);
    int **sequence = generateDiagonalSequence(sequence_length, hyper_width * hyper_height);
    saveSequence(sequence, sequence_length, hyper_width * hyper_height,"sequence" + output_name + ".csv");

    initializeSparse();

    // trace recording
#ifdef TRACES
    FILE *traceAmpa = fopen("trace_ampa.csv", "w");
    FILE *traceNmda = fopen("trace_nmda.csv", "w");
#endif

    // vmem recording
#ifdef VMEM
    FILE *traceVmem = fopen("trace_vmem.csv", "w");
#endif

    // SETTLE
    setGainAndKappa(0.0, 1.0);
    setAllStimulation(background_freq);
    t_start = t;
    while (t - t_start < settle_time)
    {
        stepTime();
#ifdef TRACES
        pullH0_0_to_H0_0_lateral_ampaStateFromDevice();
        pullH0_0_to_H0_0_lateral_nmdaStateFromDevice();
        RECORD_TRACE;
#endif
#ifdef VMEM
        pullH0_0StateFromDevice();
        recordVmem(traceVmem, VH0_0);
#endif
    }

    // TRAINING
    // t is current simulation time provided by GeNN in ms
    setGainAndKappa(0.0, 1.0); // set weight and learning rate - training
    for (int ep = 0; ep < epochs; ep++)
    {
        std::cout << "Training epoch " << ep + 1 << std::endl;

        for (int pat = 0; pat < sequence_length; pat++)
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
#ifdef VMEM
                pullH0_0StateFromDevice();
                recordVmem(traceVmem, VH0_0);
#endif
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
#ifdef VMEM
                pullH0_0StateFromDevice();
                recordVmem(traceVmem, VH0_0);
#endif
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
#ifdef TRACES
        pullH0_0_to_H0_0_lateral_ampaStateFromDevice();
        pullH0_0_to_H0_0_lateral_nmdaStateFromDevice();
        RECORD_TRACE;
#endif
#ifdef VMEM
        pullH0_0StateFromDevice();
        recordVmem(traceVmem, VH0_0);
#endif
    }
#ifdef TRACES
    fclose(traceAmpa);
    fclose(traceNmda);
#endif
#ifdef VMEM
    fclose(traceVmem);
#endif
    pullRecordingBuffersFromDevice();
    writeTextSpikeArrayRecording("output.spikes" + output_name + ".csv", recordSpkArray, std::end(recordSpkArray) - std::begin(recordSpkArray),
                                 N_minicolumns * N_pyramidal, int(buffer_time / time_step), time_step, " ", false, false);
    writeTextSpikeArrayRecording("output.basketSpikes" + output_name + ".csv", recordBasketSpkArray, std::end(recordBasketSpkArray) - std::begin(recordBasketSpkArray),
                                 N_basket, int(buffer_time / time_step), time_step, " ", false, false);
    
    recordWeights();

    return 0;
}