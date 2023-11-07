// tenLIFRingModel simulation code
#include "hypercolumns_CODE/definitions.h"
#include <fstream>

int main()
{
    allocateMem(); // allocate memory for all neuron variables
    initialize();  // initialize variables and start cpu/gpu kernel

    int N_pyramidal = 10; // usually = N_pyramidal
    allocatefiringProbH0_0_M0_input(N_pyramidal);
    allocatefiringProbH0_0_M1_input(N_pyramidal);
    allocatefiringProbH0_0_M2_input(N_pyramidal);
    allocatefiringProbH0_0_M3_input(N_pyramidal);
    allocatefiringProbH0_0_M4_input(N_pyramidal);
    allocatefiringProbH0_0_M5_input(N_pyramidal);
    allocatefiringProbH0_0_M6_input(N_pyramidal);
    allocatefiringProbH0_0_M7_input(N_pyramidal);
    allocatefiringProbH0_0_M8_input(N_pyramidal);
    allocatefiringProbH0_0_M9_input(N_pyramidal);
    for (int i = 0; i < N_pyramidal; i++)
    {
        firingProbH0_0_M0_input[i] = 0.1 * 1e-3 * 10000.0; // dT (ms) / 1000 ms * average spike frequency
        firingProbH0_0_M1_input[i] = 0.1 * 1e-3 * 350.0;
        firingProbH0_0_M2_input[i] = 0.1 * 1e-3 * 350.0;
        firingProbH0_0_M3_input[i] = 0.1 * 1e-3 * 350.0;
        firingProbH0_0_M4_input[i] = 0.1 * 1e-3 * 350.0;
        firingProbH0_0_M5_input[i] = 0.1 * 1e-3 * 350.0;
        firingProbH0_0_M6_input[i] = 0.1 * 1e-3 * 350.0;
        firingProbH0_0_M7_input[i] = 0.1 * 1e-3 * 350.0;
        firingProbH0_0_M8_input[i] = 0.1 * 1e-3 * 350.0;
        firingProbH0_0_M9_input[i] = 0.1 * 1e-3 * 350.0;
    }
    pushfiringProbH0_0_M0_inputToDevice(N_pyramidal);
    pushfiringProbH0_0_M1_inputToDevice(N_pyramidal);
    pushfiringProbH0_0_M2_inputToDevice(N_pyramidal);
    pushfiringProbH0_0_M3_inputToDevice(N_pyramidal);
    pushfiringProbH0_0_M4_inputToDevice(N_pyramidal);
    pushfiringProbH0_0_M5_inputToDevice(N_pyramidal);
    pushfiringProbH0_0_M6_inputToDevice(N_pyramidal);
    pushfiringProbH0_0_M7_inputToDevice(N_pyramidal);
    pushfiringProbH0_0_M8_inputToDevice(N_pyramidal);
    pushfiringProbH0_0_M9_inputToDevice(N_pyramidal);

    initializeSparse();

    // t is current simulation time provided by GeNN in ms
    std::ofstream os("output.V.dat");
    std::ofstream os_spike("output.spikes.dat");
    bool switched1 = false;
    while (t < 200.0f)
    {
        if (t > 20.0f && !switched1)
        {
            for (int i = 0; i < N_pyramidal; i++)
            {
                firingProbH0_0_M0_input[i] = 0.1 * 1e-3 * 350.0;
            }
            pushfiringProbH0_0_M0_inputToDevice(N_pyramidal); // argument 0: count
            switched1 = true;
        }

        stepTime();
        pullVH0_0_M0FromDevice(); // get voltages
        pullVH0_1_M0FromDevice();
        pullVH1_0_M0FromDevice();
        pullVH1_1_M0FromDevice();
        pullVH0_0_basketsFromDevice();
        pullH0_0_M0_inputCurrentSpikesFromDevice(); // get spikes
        pullH0_0_M0CurrentSpikesFromDevice();
        pullH0_1_M0CurrentSpikesFromDevice();
        pullH1_0_M0CurrentSpikesFromDevice();
        pullH1_1_M0CurrentSpikesFromDevice();
        pullH0_0_M1_inputCurrentSpikesFromDevice();
        pullH0_0_M1CurrentSpikesFromDevice();
        pullH0_1_M1CurrentSpikesFromDevice();
        pullH1_0_M1CurrentSpikesFromDevice();
        pullH1_1_M1CurrentSpikesFromDevice();
        pullH0_0_M2_inputCurrentSpikesFromDevice();
        pullH0_0_M2CurrentSpikesFromDevice();
        pullH0_1_M2CurrentSpikesFromDevice();
        pullH1_0_M2CurrentSpikesFromDevice();
        pullH1_1_M2CurrentSpikesFromDevice();
        pullH0_0_basketsCurrentSpikesFromDevice();
        // pullH0_0_M0SpikesFromDevice();
        //  pullPop1CurrentSpikesFromDevice();
        os << t << " ";
        os_spike << t << " ";
        for (int j = 0; j < 10; j++)
        {
            os << VH0_1_M0[j] << " ";
        }
        os_spike << spikeCount_H0_0_M0_input << " ";
        os_spike << spikeCount_H0_0_M0 << " ";
        os_spike << spikeCount_H0_1_M0 << " ";
        os_spike << spikeCount_H1_0_M0 << " ";
        os_spike << spikeCount_H1_1_M0 << " ";
        os_spike << spikeCount_H0_0_M1_input << " ";
        os_spike << spikeCount_H0_0_M1 << " ";
        os_spike << spikeCount_H0_1_M1 << " ";
        os_spike << spikeCount_H1_0_M1 << " ";
        os_spike << spikeCount_H1_1_M1 << " ";
        os_spike << spikeCount_H0_0_M2_input << " ";
        os_spike << spikeCount_H0_0_M2 << " ";
        os_spike << spikeCount_H0_1_M2 << " ";
        os_spike << spikeCount_H1_0_M2 << " ";
        os_spike << spikeCount_H1_1_M2 << " ";
        

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