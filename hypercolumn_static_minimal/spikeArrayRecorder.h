#pragma once

// Standard C++ includes
#include <algorithm>
#include <fstream>
#include <iterator>
#include <list>
#include <string>
#include <tuple>
#include <vector>

// Standard C includes
#include <cassert>
#include <cstdint>

#ifdef _WIN32
#include <intrin.h>
#endif



inline void writeTextSpikeArrayRecording(const std::string &filename, uint32_t ***spkRecordArray, unsigned int arrayLength,
                                    unsigned int popSize, unsigned int numTimesteps, double dt = 1.0,
                                    const std::string &delimiter = " ", bool header = false, bool append = false, 
                                    double startTime = 0.0)
{
    // Calculate number of words per-timestep
    const unsigned int timestepWords = (popSize + 31) / 32;
    
    // Create stream and set precision
    std::ofstream stream(filename, append ? std::ofstream::app : std::ofstream::out);
    stream.precision(16);
    
    // Write header if required
    if(header) {
        stream << "Time [ms], Neuron ID" << std::endl;
    }
    uint32_t *spkRecord; 
    // Loop through populations
    for(unsigned int p = 0; p < arrayLength; p++){
        spkRecord = *spkRecordArray[p];
        // Loop through timesteps
        for(unsigned int t = 0; t < numTimesteps; t++) {
            // Convert timestep to time
            const double time = startTime + (t * dt);
            
            // Loop through words representing timestep
            for(unsigned int w = 0; w < timestepWords; w++) {
                // Get word
                uint32_t spikeWord = spkRecord[(t * timestepWords) + w];
                
                // Calculate neuron id of highest bit of this word
                unsigned int neuronID = (w * 32) + 31;
                
                // While bits remain
                while(spikeWord != 0) {
                    // Calculate leading zeros
                    const int numLZ = _clz(spikeWord);
                    
                    // If all bits have now been processed, zero spike word
                    // Otherwise shift past the spike we have found
                    spikeWord = (numLZ == 31) ? 0 : (spikeWord << (numLZ + 1));
                    
                    // Subtract number of leading zeros from neuron ID
                    neuronID -= numLZ;
                    
                    // Write out CSV line
                    stream << time << delimiter << p*popSize+neuronID << std::endl;
                    
                    // New neuron id of the highest bit of this word
                    neuronID--;
                }
            }
        }
    }
}
