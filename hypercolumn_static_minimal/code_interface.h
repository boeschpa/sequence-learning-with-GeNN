#include "hypercolumn_CODE/definitions.h"
typedef void (*allocatefiringProb)(unsigned int); 
allocatefiringProb allocatefiringProbs[] = {
    allocatefiringProbH0_0_M0_input,
    allocatefiringProbH0_0_M1_input,
    allocatefiringProbH0_0_M2_input,
    allocatefiringProbH0_0_M3_input,
    allocatefiringProbH0_0_M4_input,
    allocatefiringProbH0_1_M0_input,
    allocatefiringProbH0_1_M1_input,
    allocatefiringProbH0_1_M2_input,
    allocatefiringProbH0_1_M3_input,
    allocatefiringProbH0_1_M4_input,
};
typedef void (*pushfiringProbToDevice)(unsigned int); 
pushfiringProbToDevice pushfiringProbsToDevice[] = {
    pushfiringProbH0_0_M0_inputToDevice,
    pushfiringProbH0_0_M1_inputToDevice,
    pushfiringProbH0_0_M2_inputToDevice,
    pushfiringProbH0_0_M3_inputToDevice,
    pushfiringProbH0_0_M4_inputToDevice,
    pushfiringProbH0_1_M0_inputToDevice,
    pushfiringProbH0_1_M1_inputToDevice,
    pushfiringProbH0_1_M2_inputToDevice,
    pushfiringProbH0_1_M3_inputToDevice,
    pushfiringProbH0_1_M4_inputToDevice,
};
scalar** firingProbs[] = {
    &firingProbH0_0_M0_input,
    &firingProbH0_0_M1_input,
    &firingProbH0_0_M2_input,
    &firingProbH0_0_M3_input,
    &firingProbH0_0_M4_input,
    &firingProbH0_1_M0_input,
    &firingProbH0_1_M1_input,
    &firingProbH0_1_M2_input,
    &firingProbH0_1_M3_input,
    &firingProbH0_1_M4_input,
};
uint32_t** recordSpkArray[] = {
    &recordSpkH0_0_M0,
    &recordSpkH0_0_M1,
    &recordSpkH0_0_M2,
    &recordSpkH0_0_M3,
    &recordSpkH0_0_M4,
    &recordSpkH0_1_M0,
    &recordSpkH0_1_M1,
    &recordSpkH0_1_M2,
    &recordSpkH0_1_M3,
    &recordSpkH0_1_M4,
};
