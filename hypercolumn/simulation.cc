// tenLIFRingModel simulation code
#include "hypercolumns_CODE/definitions.h"
#include <fstream>

int main()
{
    allocateMem(); // allocate memory for all neuron variables
    initialize();  // initialize variables and start cpu/gpu kernel

    int N_pyramidal = 10; // usually = N_pyramidal
    float stim_freq = 100.0;
    float stim_time = 10.0;
    float background_freq = 100.0;

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

    allocatefiringProbH0_1_M0_input(N_pyramidal);
    allocatefiringProbH0_1_M1_input(N_pyramidal);
    allocatefiringProbH0_1_M2_input(N_pyramidal);
    allocatefiringProbH0_1_M3_input(N_pyramidal);
    allocatefiringProbH0_1_M4_input(N_pyramidal);
    allocatefiringProbH0_1_M5_input(N_pyramidal);
    allocatefiringProbH0_1_M6_input(N_pyramidal);
    allocatefiringProbH0_1_M7_input(N_pyramidal);
    allocatefiringProbH0_1_M8_input(N_pyramidal);
    allocatefiringProbH0_1_M9_input(N_pyramidal);

    allocatefiringProbH1_0_M0_input(N_pyramidal);
    allocatefiringProbH1_0_M1_input(N_pyramidal);
    allocatefiringProbH1_0_M2_input(N_pyramidal);
    allocatefiringProbH1_0_M3_input(N_pyramidal);
    allocatefiringProbH1_0_M4_input(N_pyramidal);
    allocatefiringProbH1_0_M5_input(N_pyramidal);
    allocatefiringProbH1_0_M6_input(N_pyramidal);
    allocatefiringProbH1_0_M7_input(N_pyramidal);
    allocatefiringProbH1_0_M8_input(N_pyramidal);
    allocatefiringProbH1_0_M9_input(N_pyramidal);

    allocatefiringProbH1_1_M0_input(N_pyramidal);
    allocatefiringProbH1_1_M1_input(N_pyramidal);
    allocatefiringProbH1_1_M2_input(N_pyramidal);
    allocatefiringProbH1_1_M3_input(N_pyramidal);
    allocatefiringProbH1_1_M4_input(N_pyramidal);
    allocatefiringProbH1_1_M5_input(N_pyramidal);
    allocatefiringProbH1_1_M6_input(N_pyramidal);
    allocatefiringProbH1_1_M7_input(N_pyramidal);
    allocatefiringProbH1_1_M8_input(N_pyramidal);
    allocatefiringProbH1_1_M9_input(N_pyramidal);
    for (int i = 0; i < N_pyramidal; i++)
    {
        firingProbH0_0_M0_input[i] = 0.1 * 1e-3 * stim_freq; // dT (ms) / 1000 ms * average spike frequency
        firingProbH0_0_M1_input[i] = 0.1 * 1e-3 * background_freq;
        firingProbH0_0_M2_input[i] = 0.1 * 1e-3 * background_freq;
        firingProbH0_0_M3_input[i] = 0.1 * 1e-3 * background_freq;
        firingProbH0_0_M4_input[i] = 0.1 * 1e-3 * background_freq;
        firingProbH0_0_M5_input[i] = 0.1 * 1e-3 * background_freq;
        firingProbH0_0_M6_input[i] = 0.1 * 1e-3 * background_freq;
        firingProbH0_0_M7_input[i] = 0.1 * 1e-3 * background_freq;
        firingProbH0_0_M8_input[i] = 0.1 * 1e-3 * background_freq;
        firingProbH0_0_M9_input[i] = 0.1 * 1e-3 * background_freq;

        firingProbH0_1_M0_input[i] = 0.1 * 1e-3 * stim_freq;
        firingProbH0_1_M1_input[i] = 0.1 * 1e-3 * background_freq;
        firingProbH0_1_M2_input[i] = 0.1 * 1e-3 * background_freq;
        firingProbH0_1_M3_input[i] = 0.1 * 1e-3 * background_freq;
        firingProbH0_1_M4_input[i] = 0.1 * 1e-3 * background_freq;
        firingProbH0_1_M5_input[i] = 0.1 * 1e-3 * background_freq;
        firingProbH0_1_M6_input[i] = 0.1 * 1e-3 * background_freq;
        firingProbH0_1_M7_input[i] = 0.1 * 1e-3 * background_freq;
        firingProbH0_1_M8_input[i] = 0.1 * 1e-3 * background_freq;
        firingProbH0_1_M9_input[i] = 0.1 * 1e-3 * background_freq;

        firingProbH1_0_M0_input[i] = 0.1 * 1e-3 * stim_freq;
        firingProbH1_0_M1_input[i] = 0.1 * 1e-3 * background_freq;
        firingProbH1_0_M2_input[i] = 0.1 * 1e-3 * background_freq;
        firingProbH1_0_M3_input[i] = 0.1 * 1e-3 * background_freq;
        firingProbH1_0_M4_input[i] = 0.1 * 1e-3 * background_freq;
        firingProbH1_0_M5_input[i] = 0.1 * 1e-3 * background_freq;
        firingProbH1_0_M6_input[i] = 0.1 * 1e-3 * background_freq;
        firingProbH1_0_M7_input[i] = 0.1 * 1e-3 * background_freq;
        firingProbH1_0_M8_input[i] = 0.1 * 1e-3 * background_freq;
        firingProbH1_0_M9_input[i] = 0.1 * 1e-3 * background_freq;

        firingProbH1_1_M0_input[i] = 0.1 * 1e-3 * stim_freq;
        firingProbH1_1_M1_input[i] = 0.1 * 1e-3 * background_freq;
        firingProbH1_1_M2_input[i] = 0.1 * 1e-3 * background_freq;
        firingProbH1_1_M3_input[i] = 0.1 * 1e-3 * background_freq;
        firingProbH1_1_M4_input[i] = 0.1 * 1e-3 * background_freq;
        firingProbH1_1_M5_input[i] = 0.1 * 1e-3 * background_freq;
        firingProbH1_1_M6_input[i] = 0.1 * 1e-3 * background_freq;
        firingProbH1_1_M7_input[i] = 0.1 * 1e-3 * background_freq;
        firingProbH1_1_M8_input[i] = 0.1 * 1e-3 * background_freq;
        firingProbH1_1_M9_input[i] = 0.1 * 1e-3 * background_freq;
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

    pushfiringProbH0_1_M0_inputToDevice(N_pyramidal);
    pushfiringProbH0_1_M1_inputToDevice(N_pyramidal);
    pushfiringProbH0_1_M2_inputToDevice(N_pyramidal);
    pushfiringProbH0_1_M3_inputToDevice(N_pyramidal);
    pushfiringProbH0_1_M4_inputToDevice(N_pyramidal);
    pushfiringProbH0_1_M5_inputToDevice(N_pyramidal);
    pushfiringProbH0_1_M6_inputToDevice(N_pyramidal);
    pushfiringProbH0_1_M7_inputToDevice(N_pyramidal);
    pushfiringProbH0_1_M8_inputToDevice(N_pyramidal);
    pushfiringProbH0_1_M9_inputToDevice(N_pyramidal);

    pushfiringProbH1_0_M0_inputToDevice(N_pyramidal);
    pushfiringProbH1_0_M1_inputToDevice(N_pyramidal);
    pushfiringProbH1_0_M2_inputToDevice(N_pyramidal);
    pushfiringProbH1_0_M3_inputToDevice(N_pyramidal);
    pushfiringProbH1_0_M4_inputToDevice(N_pyramidal);
    pushfiringProbH1_0_M5_inputToDevice(N_pyramidal);
    pushfiringProbH1_0_M6_inputToDevice(N_pyramidal);
    pushfiringProbH1_0_M7_inputToDevice(N_pyramidal);
    pushfiringProbH1_0_M8_inputToDevice(N_pyramidal);
    pushfiringProbH1_0_M9_inputToDevice(N_pyramidal);

    pushfiringProbH1_1_M0_inputToDevice(N_pyramidal);
    pushfiringProbH1_1_M1_inputToDevice(N_pyramidal);
    pushfiringProbH1_1_M2_inputToDevice(N_pyramidal);
    pushfiringProbH1_1_M3_inputToDevice(N_pyramidal);
    pushfiringProbH1_1_M4_inputToDevice(N_pyramidal);
    pushfiringProbH1_1_M5_inputToDevice(N_pyramidal);
    pushfiringProbH1_1_M6_inputToDevice(N_pyramidal);
    pushfiringProbH1_1_M7_inputToDevice(N_pyramidal);
    pushfiringProbH1_1_M8_inputToDevice(N_pyramidal);
    pushfiringProbH1_1_M9_inputToDevice(N_pyramidal);

    initializeSparse();

    // t is current simulation time provided by GeNN in ms
    std::ofstream os("output.V.dat");
    std::ofstream os_spike("output.spikes.dat");
    bool switched1 = false;
    while (t < 5000.0f)
    {
        if (t > stim_time && !switched1)
        {
            for (int i = 0; i < N_pyramidal; i++)
            {
                firingProbH0_0_M0_input[i] = 0.1 * 1e-3 * background_freq;
                firingProbH0_1_M0_input[i] = 0.1 * 1e-3 * background_freq;
                firingProbH1_0_M0_input[i] = 0.1 * 1e-3 * background_freq;
                firingProbH1_1_M0_input[i] = 0.1 * 1e-3 * background_freq;
            }
            pushfiringProbH0_0_M0_inputToDevice(N_pyramidal); // argument 0: count
            pushfiringProbH0_1_M0_inputToDevice(N_pyramidal); // argument 0: count
            pushfiringProbH1_0_M0_inputToDevice(N_pyramidal); // argument 0: count
            pushfiringProbH1_1_M0_inputToDevice(N_pyramidal); // argument 0: count
            switched1 = true;
        }

        stepTime();
        pullVH0_0_M0FromDevice(); // get voltages
        pullVH0_1_M0FromDevice();
        pullVH1_0_M0FromDevice();
        pullVH1_1_M0FromDevice();
        pullVH0_0_basketsFromDevice();

        // get spikes
        pullH0_0_M0CurrentSpikesFromDevice();
        pullH0_1_M0CurrentSpikesFromDevice();
        pullH1_0_M0CurrentSpikesFromDevice();
        pullH1_1_M0CurrentSpikesFromDevice();
        pullH0_0_M0_inputCurrentSpikesFromDevice();
        pullH0_0_M1CurrentSpikesFromDevice();
        pullH0_1_M1CurrentSpikesFromDevice();
        pullH1_0_M1CurrentSpikesFromDevice();
        pullH1_1_M1CurrentSpikesFromDevice();
        pullH0_0_M1_inputCurrentSpikesFromDevice();
        pullH0_0_M2CurrentSpikesFromDevice();
        pullH0_1_M2CurrentSpikesFromDevice();
        pullH1_0_M2CurrentSpikesFromDevice();
        pullH1_1_M2CurrentSpikesFromDevice();
        pullH0_0_M2_inputCurrentSpikesFromDevice();
        pullH0_0_M3CurrentSpikesFromDevice();
        pullH0_1_M3CurrentSpikesFromDevice();
        pullH1_0_M3CurrentSpikesFromDevice();
        pullH1_1_M3CurrentSpikesFromDevice();
        pullH0_0_M3_inputCurrentSpikesFromDevice();
        pullH0_0_M4CurrentSpikesFromDevice();
        pullH0_1_M4CurrentSpikesFromDevice();
        pullH1_0_M4CurrentSpikesFromDevice();
        pullH1_1_M4CurrentSpikesFromDevice();
        pullH0_0_M4_inputCurrentSpikesFromDevice();
        pullH0_0_M5CurrentSpikesFromDevice();
        pullH0_1_M5CurrentSpikesFromDevice();
        pullH1_0_M5CurrentSpikesFromDevice();
        pullH1_1_M5CurrentSpikesFromDevice();
        pullH0_0_M5_inputCurrentSpikesFromDevice();
        pullH0_0_M6CurrentSpikesFromDevice();
        pullH0_1_M6CurrentSpikesFromDevice();
        pullH1_0_M6CurrentSpikesFromDevice();
        pullH1_1_M6CurrentSpikesFromDevice();
        pullH0_0_M6_inputCurrentSpikesFromDevice();
        pullH0_0_M7CurrentSpikesFromDevice();
        pullH0_1_M7CurrentSpikesFromDevice();
        pullH1_0_M7CurrentSpikesFromDevice();
        pullH1_1_M7CurrentSpikesFromDevice();
        pullH0_0_M7_inputCurrentSpikesFromDevice();
        pullH0_0_M8CurrentSpikesFromDevice();
        pullH0_1_M8CurrentSpikesFromDevice();
        pullH1_0_M8CurrentSpikesFromDevice();
        pullH1_1_M8CurrentSpikesFromDevice();
        pullH0_0_M8_inputCurrentSpikesFromDevice();
        pullH0_0_M9CurrentSpikesFromDevice();
        pullH0_1_M9CurrentSpikesFromDevice();
        pullH1_0_M9CurrentSpikesFromDevice();
        pullH1_1_M9CurrentSpikesFromDevice();
        pullH0_0_M9_inputCurrentSpikesFromDevice();
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
        os_spike << spikeCount_H0_0_M3_input << " ";
        os_spike << spikeCount_H0_0_M3 << " ";
        os_spike << spikeCount_H0_1_M3 << " ";
        os_spike << spikeCount_H1_0_M3 << " ";
        os_spike << spikeCount_H1_1_M3 << " ";
        os_spike << spikeCount_H0_0_M4_input << " ";
        os_spike << spikeCount_H0_0_M4 << " ";
        os_spike << spikeCount_H0_1_M4 << " ";
        os_spike << spikeCount_H1_0_M4 << " ";
        os_spike << spikeCount_H1_1_M4 << " ";
        os_spike << spikeCount_H0_0_M5_input << " ";
        os_spike << spikeCount_H0_0_M5 << " ";
        os_spike << spikeCount_H0_1_M5 << " ";
        os_spike << spikeCount_H1_0_M5 << " ";
        os_spike << spikeCount_H1_1_M5 << " ";
        os_spike << spikeCount_H0_0_M6_input << " ";
        os_spike << spikeCount_H0_0_M6 << " ";
        os_spike << spikeCount_H0_1_M6 << " ";
        os_spike << spikeCount_H1_0_M6 << " ";
        os_spike << spikeCount_H1_1_M6 << " ";
        os_spike << spikeCount_H0_0_M7_input << " ";
        os_spike << spikeCount_H0_0_M7 << " ";
        os_spike << spikeCount_H0_1_M7 << " ";
        os_spike << spikeCount_H1_0_M7 << " ";
        os_spike << spikeCount_H1_1_M7 << " ";
        os_spike << spikeCount_H0_0_M8_input << " ";
        os_spike << spikeCount_H0_0_M8 << " ";
        os_spike << spikeCount_H0_1_M8 << " ";
        os_spike << spikeCount_H1_0_M8 << " ";
        os_spike << spikeCount_H1_1_M8 << " ";
        os_spike << spikeCount_H0_0_M9_input << " ";
        os_spike << spikeCount_H0_0_M9 << " ";
        os_spike << spikeCount_H0_1_M9 << " ";
        os_spike << spikeCount_H1_0_M9 << " ";
        os_spike << spikeCount_H1_1_M9 << " ";

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