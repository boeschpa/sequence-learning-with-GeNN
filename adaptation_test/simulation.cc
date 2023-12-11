#include <algorithm>
#include <iostream>
#include <random>

#include <cassert>
#include <cstdint>

// Userproject includes
#include "spikeRecorder.h"

#include "test_CODE/definitions.h"
#include "model_param.h"

namespace
{
    void printPattern(const std::vector<bool> &p)
    {
        for (bool a : p)
        {
            std::cout << a << ", ";
        }
        std::cout << std::endl;
    }

    template <typename R>
    std::vector<bool> createActivePattern(size_t num, R &rng)
    {
        std::vector<bool> p;
        p.reserve(num);

        std::uniform_int_distribution<int> distribution(0, 1);
        std::generate_n(std::back_inserter(p), num,
                        [&rng, &distribution]()
                        { return (distribution(rng) != 0); });
        return p;
    }

    std::vector<bool> createInactivePattern(size_t num)
    {
        std::vector<bool> p(num, false);
        return p;
    }

    std::vector<bool> createUncorrelatedPattern(const std::vector<bool> &p)
    {
        std::vector<bool> u;
        u.reserve(p.size());

        std::transform(std::begin(p), std::end(p), std::back_inserter(u),
                       [](bool p)
                       { return !p; });
        return u;
    }

    std::vector<bool> createCorrelatedPattern(const std::vector<bool> &p)
    {
        std::vector<bool> c(p);
        return c;
    }

    template <typename R, size_t N>
    std::vector<bool> createSpikeVector(const std::vector<bool> (&data)[N], float fmax, size_t delayTimesteps, size_t patternTimesteps, R &rng)
    {
        // Reserve spike vector
        std::vector<bool> spikes;

        // Add delay to start
        spikes.insert(spikes.end(), delayTimesteps, false);

        std::uniform_real_distribution<float> distribution(0, 1);

        // Loop through patterns
        for (const auto &p : data)
        {
            for (bool a : p)
            {
                // Get firing frequency for pattern and corresponding threshold
                const float freq = a ? fmax : 0.0f;
                const float thresh = freq * 0.001f;

                // Generate spike vector
                std::generate_n(std::back_inserter(spikes), patternTimesteps,
                                [thresh, &rng, &distribution]()
                                { return (thresh > distribution(rng)); });
            }
        }

        return spikes;
    }
} // Anonymous namespace

int main()
{
    const size_t numPatterns = 10;
    std::default_random_engine generator;

    // Create patterns
    const auto active1 = createActivePattern(numPatterns, generator);
    const auto active2 = createActivePattern(numPatterns, generator);

    const auto inactive0 = createInactivePattern(numPatterns);

    // Create spike vectors
    const std::vector<bool> pre[] = {active1};
    const std::vector<bool> post[] = {inactive0}; // only stimulate pre
    const auto preSpikeVector = createSpikeVector(pre, 50.0f, 20, 180, generator);
    const auto postSpikeVector = createSpikeVector(post, 50.0f, 20, 180, generator);
    assert(preSpikeVector.size() == postSpikeVector.size());

    allocateMem();
    allocateRecordingBuffers(preSpikeVector.size());

    initialize();
    initializeSparse();

    // Load spike vectors
    std::cout << "Sim timesteps:" << preSpikeVector.size() << std::endl;

    FILE *preStimulus = fopen("pre_stim.csv", "w");
    for (int i=0; i<preSpikeVector.size(); i++){
        
    }

    FILE *preTrace = fopen("pre_trace.csv", "w");
    FILE *postTrace = fopen("post_trace.csv", "w");

    // freeze neurons and synapses
    biasGainPre[0] = 0.0;
    pushbiasGainPreToDevice();

    kappaPre[0] = 0.0;
    pushkappaPreToDevice();

    wGainPreToPost[0] = 1.0;
    pushwGainPreToPostToDevice();

    kappaPreToPost[0] = 0.0;
    pushkappaPreToPostToDevice();

    biasGainPost[0] = 0.0;
    pushbiasGainPostToDevice();

    kappaPost[0] = 0.0;
    pushkappaPostToDevice();

    // Loop through timesteps
    bool recordPreTrace = false;
    bool recordPostTrace = false;
    bool recall = false;
    for (unsigned int i = 0; i < preSpikeVector.size(); i++)
    {
        // Apply input specified by spike vector
        glbSpkCntPreStim[0] = 0;
        glbSpkCntPostStim[0] = 0;
        if (preSpikeVector[i])
        {
            glbSpkPreStim[glbSpkCntPreStim[0]++] = 0;
            fprintf(preStimulus, "%i 0\n", (int)t);
        }
        if (postSpikeVector[i])
        {
            glbSpkPostStim[glbSpkCntPostStim[0]++] = 0;
        }

        // Simulate
        pushPreStimCurrentSpikesToDevice();
        pushPostStimCurrentSpikesToDevice();

        stepTime();

        // **YUCK** only for recordXXXTrace logic
        pullPreCurrentSpikesFromDevice();
        pullPostCurrentSpikesFromDevice();

        pullPreToPostStateFromDevice();

        // RECORD TRACES

#ifdef cadex
        fprintf(preTrace, "%f, %f, %f\n", t, VPre[0], GaPre[0]);
        fprintf(postTrace, "%f, %f, %f\n", t, VPost[0], GaPost[0]);
#else
        fprintf(preTrace, "%f, %f, %f\n", t, VPre[0], IwPre[0]);
        fprintf(postTrace, "%f, %f, %f\n", t, VPost[0], IwPost[0]);
#endif
    }

    pullRecordingBuffersFromDevice();
    writeTextSpikeRecording("pre_spikes.csv", recordSpkPre,
                            1, preSpikeVector.size());
    writeTextSpikeRecording("post_spikes.csv", recordSpkPost,
                            1, preSpikeVector.size());
    fclose(preStimulus);
    fclose(preTrace);
    fclose(postTrace);

    return 0;
}