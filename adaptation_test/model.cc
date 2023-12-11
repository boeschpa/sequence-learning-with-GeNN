// GeNN includes
#include "modelSpec.h"

// GeNN examples includes
#include "SimpleAdEx.h"
#include "CAdEx.h"
#include "model_param.h"
#include "bcpnn.h"

void modelDefinition(NNmodel &model)
{
    model.setDT(1.0);
    model.setName("test");

    //---------------------------------------------------------------------------
    // Build model
    //---------------------------------------------------------------------------
    // LIF model parameters
    SimpleAdEx::ParamValues p_pyramidal(
        0.28,    // 0 - Membrane capacitance [pF] 280 pf
        14.0e-3, // 1 - Membrane leak conductance [uS] 14 ns
        -70.0,   // 2 - Resting membrane potential [mV]
        -70.0,   // 3 - Reset voltage [mV]
        -55.0,   // 4 - Spiking threshold [mV]
        3.0,     // 5 - spike upstroke slopefactor [mV]
        150.0,   // 6 - adaption time constant [ms]
        0.15,    // 7 - adatpion current per spike [nA]  (150 pA)
        5.0,     // TauZ
        5000.0,  // TauP
        0.001,   // epsilon
        1.0,     // deltaT
        50,      // fMax
        0.05     // biasGainBase 50 pA
    );

    CAdEx::ParamValues p_pyramidal_CAdEx(
        0.28,    // 0 - Membrane capacitance [pF] 280 pf
        14.0e-3, // 1 - Membrane leak conductance [uS] 14 ns
        -70.0,   // 2 - Resting membrane potential [mV]
        -70.0,   // 3 - Reset voltage [mV]
        -55.0,   // 4 - Spiking threshold [mV]
        3.0,     // 5 - spike upstroke slopefactor [mV]
        150.0,   // 6 - adaption time constant [ms]
        0.001,  // 7 adaptation conductance per spike [uS]
        -70.0,   // 8 adaptation reversal potential (mV)
        0.0,     // 9 subthreshold adaptation activation voltage (mV)
        1.0,     // 10 slope of subthreshold adaptation (mV)
        0.0,     // 11 max subthreshold adaptation conductance (uS)
        5.0,     // TauZ
        5000.0,  // TauP
        0.001,   // epsilon
        1.0,     // deltaT
        50,      // fMax
        0.05     // biasGainBase 50 pA
    );

    SimpleAdEx::VarValues ini_pyramidal(
        -70.0, // 0 - membrane potential V [mV]
        0.0,   // 1 - Ga adaption conductance [uS]
        0.0,   // Zj
        0.01,  // Pj
        0.0,   // Ib
        1.0,   // kappa
        0.0    // biasGain
    );

    CAdEx::VarValues ini_pyramidal_CAdEx(
        -70.0, // 0 - membrane potential V [mV]
        0.0,   // 1 - Iw adaption current [pA]
        0.0,   // Zj
        0.01,  // Pj
        0.0,   // Ib
        1.0,   // kappa
        0.0    // biasGain
    );

    WeightUpdateModels::StaticPulse::VarValues staticSynapseInit(
        0.1); // 0 - Wij (nA)

    BCPNN::ParamValues bcpnn_p(
        5.0,    // 0 - Time constant of presynaptic primary trace (ms)
        5.0,    // 1 - Time constant of postsynaptic primary trace (ms)
        1000.0, // 2 - Time constant of probability trace
        100.0,  // 3 - Maximum firing frequency (Hz)
        1.0,    // 5 - spike duration (ms)
        0.01,   // 6 - epsilon
        800.0,  // 7 - short term depression time constant
        0.25);  // 7 - depletion fraction

    BCPNN::VarValues bcpnn_ini(
        0.0,     // 0 - g
        0.01,    // 1 - PijStar
        0.0,     // Zi
        0.01,    // Pi
        0.0,     // Zj
        0.01,    // Pj
        0.00602, // w_gain_base
        0.0,     // w_gain
        1.0,     // kappa
        0.0,     // delay d
        1.0);    // x

    // Exponential current parameters
    PostsynapticModels::ExpCond::ParamValues ps_p(
        5.0,    // 0 - tau_S: decay time constant for S [ms]
        0.0); // 1 - Erev: Reversal potential

    // Create adex neuron
    model.addNeuronPopulation<NeuronModels::SpikeSource>("PreStim", 1, {}, {});
    model.addNeuronPopulation<NeuronModels::SpikeSource>("PostStim", 1, {}, {});

    #ifdef cadex
    auto *pre = model.addNeuronPopulation<CAdEx>("Pre", 1, p_pyramidal_CAdEx, ini_pyramidal_CAdEx);
    auto *post = model.addNeuronPopulation<CAdEx>("Post", 1, p_pyramidal_CAdEx, ini_pyramidal_CAdEx);
    #else
    auto *pre = model.addNeuronPopulation<SimpleAdEx>("Pre", 1, p_pyramidal, ini_pyramidal);
    auto *post = model.addNeuronPopulation<SimpleAdEx>("Post", 1, p_pyramidal, ini_pyramidal);
    #endif
    // enable recording
    pre->setSpikeRecordingEnabled(true);
    post->setSpikeRecordingEnabled(true);

    model.addSynapsePopulation<WeightUpdateModels::StaticPulse, PostsynapticModels::ExpCond>(
        "PreStimToPre", SynapseMatrixType::SPARSE_GLOBALG, 1,
        "PreStim", "Pre",
        {}, staticSynapseInit,
        ps_p, {},
        initConnectivity<InitSparseConnectivitySnippet::OneToOne>());
    model.addSynapsePopulation<WeightUpdateModels::StaticPulse, PostsynapticModels::ExpCond>(
        "PostStimToPost", SynapseMatrixType::SPARSE_GLOBALG, 1,
        "PostStim", "Post",
        {}, staticSynapseInit,
        ps_p, {},
        initConnectivity<InitSparseConnectivitySnippet::OneToOne>());

    model.addSynapsePopulation<BCPNN, PostsynapticModels::ExpCond>(
        "PreToPost", SynapseMatrixType::SPARSE_INDIVIDUALG, 1,
        "Pre", "Post",
        bcpnn_p, bcpnn_ini,
        ps_p, {},
        initConnectivity<InitSparseConnectivitySnippet::OneToOne>());
}