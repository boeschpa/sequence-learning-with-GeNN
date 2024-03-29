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
uint32_t** recordBasketSpkArray[] = {
    &recordSpkH0_0_baskets,
    &recordSpkH0_1_baskets,
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
scalar** biasGains[] = {
    &biasGainH0_0,
    &biasGainH0_1,
};
scalar** kappasBias[] = {
    &kappaH0_0,
    &kappaH0_1,
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
typedef void (*pushbiasGain)(bool); 
pushbiasGain pushbiasGains[] = {
    pushbiasGainH0_0ToDevice,
    pushbiasGainH0_1ToDevice,
};
typedef void (*pushkappaBias)(bool); 
pushkappaBias pushkappasBias[] = {
    pushkappaH0_0ToDevice,
    pushkappaH0_1ToDevice,
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
const unsigned int* maxRowLengthAmpas[] = {
    &maxRowLengthH0_0_to_H0_0_lateral_ampa,
    &maxRowLengthH0_0_to_H0_1_lateral_ampa,
    &maxRowLengthH0_1_to_H0_0_lateral_ampa,
    &maxRowLengthH0_1_to_H0_1_lateral_ampa,
};
const unsigned int* maxRowLengthNmdas[] = {
    &maxRowLengthH0_0_to_H0_0_lateral_nmda,
    &maxRowLengthH0_0_to_H0_1_lateral_nmda,
    &maxRowLengthH0_1_to_H0_0_lateral_nmda,
    &maxRowLengthH0_1_to_H0_1_lateral_nmda,
};
typedef void (*pullState)(); 
pullState pullStates[] = {
    &pullH0_0_to_H0_0_lateral_nmdaStateFromDevice,
    &pullH0_0_to_H0_0_lateral_ampaStateFromDevice,
    &pullH0_0_to_H0_1_lateral_nmdaStateFromDevice,
    &pullH0_0_to_H0_1_lateral_ampaStateFromDevice,
    &pullH0_1_to_H0_0_lateral_nmdaStateFromDevice,
    &pullH0_1_to_H0_0_lateral_ampaStateFromDevice,
    &pullH0_1_to_H0_1_lateral_nmdaStateFromDevice,
    &pullH0_1_to_H0_1_lateral_ampaStateFromDevice,
};
unsigned int** rowLengthAmpas[] = {
    &rowLengthH0_0_to_H0_0_lateral_ampa,
    &rowLengthH0_0_to_H0_1_lateral_ampa,
    &rowLengthH0_1_to_H0_0_lateral_ampa,
    &rowLengthH0_1_to_H0_1_lateral_ampa,
};
unsigned int** rowLengthNmdas[] = {
    &rowLengthH0_0_to_H0_0_lateral_nmda,
    &rowLengthH0_0_to_H0_1_lateral_nmda,
    &rowLengthH0_1_to_H0_0_lateral_nmda,
    &rowLengthH0_1_to_H0_1_lateral_nmda,
};
uint32_t** indAmpas[] = {
    &indH0_0_to_H0_0_lateral_ampa,
    &indH0_0_to_H0_1_lateral_ampa,
    &indH0_1_to_H0_0_lateral_ampa,
    &indH0_1_to_H0_1_lateral_ampa,
};
uint32_t** indNmdas[] = {
    &indH0_0_to_H0_0_lateral_nmda,
    &indH0_0_to_H0_1_lateral_nmda,
    &indH0_1_to_H0_0_lateral_nmda,
    &indH0_1_to_H0_1_lateral_nmda,
};
