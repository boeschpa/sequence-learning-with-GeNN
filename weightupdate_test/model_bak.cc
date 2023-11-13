// Model definintion file tenHHRing.cc
#include "modelSpec.h"
#include "StaticPulseDendriticDelayStd.h"
#include "SimpleAdEx.h"
#include "bcpnn.h"

// MODEL
void modelDefinition(ModelSpec &model)
{
    // NEURON POPULATIONS
    //  definition of tenHHRing
    model.setDT(0.1);
    model.setName("bcpnn");
    SimpleAdEx::ParamValues p_pyramidal(
        0.28,    // 0 - Membrane capacitance [pF] 280 pf
        14.0e-3, // 1 - Membrane leak conductance [uS] 14 ns
        -70.0,   // 2 - Resting membrane potential [mV]
        -70.0,   // 3 - Reset voltage [mV]
        -55.0,   // 4 - Spiking threshold [mV]
        3.0,     // 5 - spike upstroke slopefactor [mV]
        150.0,   // 6 - adaption time constant [ms]
        0.150    // 7 - adatpion current per spike [nA]  (150 pA)
    );

    SimpleAdEx::VarValues ini_pyramidal(
        -70.0, // 0 - membrane potential V [mV]
        0.0    // 1 - Iw adaption current [pA]
    );
    NeuronModels::SpikeSourceArray::VarValues stim_ini(
        uninitialisedVar(),  // 0 - startSpike indices
        uninitialisedVar()); // 1 - endSpike indices

    PostsynapticModels::ExpCond::ParamValues ps_p(
        1.0,    // 0 - tau_S: decay time constant for S [ms]
        -80.0); // 1 - Erev: Reversal potential

    // BCPNN params
    BCPNNTwoTrace::ParamValues bcpnnParams(
        10.0,   // 0 - Time constant of presynaptic primary trace (ms)
        10.0,   // 1 - Time constant of postsynaptic primary trace (ms)
        1000.0, // 2 - Time constant of probability trace
        50.0,   // 3 - Maximum firing frequency (Hz)
        1.0,    // 4 - Maximum weight
        false,  // 5 - Should weights get applied to synapses
        true);  // 6 - Should weights be updated

    BCPNNTwoTrace::VarValues bcpnnInit(
        0.0,                                   // 0 - g
        0.0,                                   // 1 - PijStar
        std::numeric_limits<float>::lowest()); // 2 - lastUpdateTime

    BCPNNTwoTrace::PreVarValues bcpnnPreInit(
        0.0,  // 0 - ZiStar
        0.0); // 1 - PiStar

    BCPNNTwoTrace::PostVarValues bcpnnPostInit(
        0.0,  // 0 - ZjStar
        0.0); // 1 - PjStar

    auto *pre  = model.addNeuronPopulation<NeuronModels::SpikeSourceArray>("Pre", 1, {}, stim_ini);
    auto *post = model.addNeuronPopulation<NeuronModels::SpikeSourceArray>("Post", 1, {}, stim_ini);
    
    // record spikes
    pre->setSpikeRecordingEnabled(true);
    post->setSpikeRecordingEnabled(true);

    auto *synPop = model.addSynapsePopulation<BCPNNTwoTrace, PostsynapticModels::ExpCond>(
        "plastic", SynapseMatrixType::SPARSE_INDIVIDUALG, NO_DELAY,
        "Pre", "Post",
        bcpnnParams, bcpnnInit, bcpnnPreInit, bcpnnPostInit,
        ps_p, {},
        initConnectivity<InitSparseConnectivitySnippet::OneToOne>());

    // Set max dendritic delay and span type
    //synPop->setMaxDendriticDelayTimesteps(std::rint(1000.0 / model.getDT()));
}