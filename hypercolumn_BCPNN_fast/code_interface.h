#include "hypercolumn_CODE/definitions.h"
typedef void (*allocatefiringProb)(unsigned int); 
allocatefiringProb allocatefiringProbs[] = {
    allocatefiringProbH0_0_input,
    allocatefiringProbH0_1_input,
};
typedef void (*pushfiringProbToDevice)(unsigned int); 
pushfiringProbToDevice pushfiringProbsToDevice[] = {
    pushfiringProbH0_0_inputToDevice,
    pushfiringProbH0_1_inputToDevice,
};
scalar** firingProbs[] = {
    &firingProbH0_0_input,
    &firingProbH0_1_input,
};
uint32_t** recordSpkArray[] = {
    &recordSpkH0_0,
    &recordSpkH0_1,
};
scalar** wGains[] = {
    &wGainH0_0_to_H0_0_lateral_nmda,
    &wGainH0_0_to_H0_0_lateral_ampa,
    &wGainH0_0_to_H0_1_lateral_nmda,
    &wGainH0_0_to_H0_1_lateral_ampa,
    &wGainH0_1_to_H0_0_lateral_nmda,
    &wGainH0_1_to_H0_0_lateral_ampa,
    &wGainH0_1_to_H0_1_lateral_nmda,
    &wGainH0_1_to_H0_1_lateral_ampa,
};
scalar** kappas[] = {
    &kappaH0_0_to_H0_0_lateral_nmda,
    &kappaH0_0_to_H0_0_lateral_ampa,
    &kappaH0_0_to_H0_1_lateral_nmda,
    &kappaH0_0_to_H0_1_lateral_ampa,
    &kappaH0_1_to_H0_0_lateral_nmda,
    &kappaH0_1_to_H0_0_lateral_ampa,
    &kappaH0_1_to_H0_1_lateral_nmda,
    &kappaH0_1_to_H0_1_lateral_ampa,
};
typedef void (*pushwGain)(bool); 
pushwGain pushwGains[] = {
    pushwGainH0_0_to_H0_0_lateral_nmdaToDevice,
    pushwGainH0_0_to_H0_0_lateral_ampaToDevice,
    pushwGainH0_0_to_H0_1_lateral_nmdaToDevice,
    pushwGainH0_0_to_H0_1_lateral_ampaToDevice,
    pushwGainH0_1_to_H0_0_lateral_nmdaToDevice,
    pushwGainH0_1_to_H0_0_lateral_ampaToDevice,
    pushwGainH0_1_to_H0_1_lateral_nmdaToDevice,
    pushwGainH0_1_to_H0_1_lateral_ampaToDevice,
};
typedef void (*pushkappa)(bool); 
pushkappa pushkappas[] = {
    pushkappaH0_0_to_H0_0_lateral_nmdaToDevice,
    pushkappaH0_0_to_H0_0_lateral_ampaToDevice,
    pushkappaH0_0_to_H0_1_lateral_nmdaToDevice,
    pushkappaH0_0_to_H0_1_lateral_ampaToDevice,
    pushkappaH0_1_to_H0_0_lateral_nmdaToDevice,
    pushkappaH0_1_to_H0_0_lateral_ampaToDevice,
    pushkappaH0_1_to_H0_1_lateral_nmdaToDevice,
    pushkappaH0_1_to_H0_1_lateral_ampaToDevice,
};
scalar** g_nmda[] = {
    &gH0_0_to_H0_0_lateral_nmda,
    &gH0_0_to_H0_1_lateral_nmda,
    &gH0_1_to_H0_0_lateral_nmda,
    &gH0_1_to_H0_1_lateral_nmda,
};
scalar** g_ampa[] = {
    &gH0_0_to_H0_0_lateral_ampa,
    &gH0_0_to_H0_1_lateral_ampa,
    &gH0_1_to_H0_0_lateral_ampa,
    &gH0_1_to_H0_1_lateral_ampa,
};
const unsigned int* maxRowLengths[] = {
    &maxRowLengthH0_0_to_H0_0_lateral_nmda,
    &maxRowLengthH0_0_to_H0_0_lateral_ampa,
    &maxRowLengthH0_0_to_H0_1_lateral_nmda,
    &maxRowLengthH0_0_to_H0_1_lateral_ampa,
    &maxRowLengthH0_1_to_H0_0_lateral_nmda,
    &maxRowLengthH0_1_to_H0_0_lateral_ampa,
    &maxRowLengthH0_1_to_H0_1_lateral_nmda,
    &maxRowLengthH0_1_to_H0_1_lateral_ampa,
};
