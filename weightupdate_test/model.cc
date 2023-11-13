// GeNN includes
#include "modelSpec.h"

// GeNN examples includes
#include "bcpnn_simple.h"
#include "SimpleAdEx.h"
#include "model_param.h"


void modelDefinition(NNmodel &model)
{
    model.setDT(1.0);
    model.setName("weightupdate");

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
        0.150    // 7 - adatpion current per spike [nA]  (150 pA)
    );

    SimpleAdEx::VarValues ini_pyramidal(
        -70.0, // 0 - membrane potential V [mV]
        0.0    // 1 - Iw adaption current [pA]
    );

    WeightUpdateModels::StaticPulse::VarValues staticSynapseInit(
        2.0);    // 0 - Wij (nA)

    // STDP values
    WeightUpdateModels::PiecewiseSTDP::ParamValues stdp_p(
        50.0,       // 0 - TLRN: time scale of learning changes
        50.0,       // 1 - TCHNG: width of learning window
        50000.0,    // 2 - TDECAY: time scale of synaptic strength decay
        100000.0,   // 3 - TPUNISH10: Time window of suppression in response to 1/0
        200.0,      // 4 - TPUNISH01: Time window of suppression in response to 0/1
        0.015,      // 5 - GMAX: Maximal conductance achievable
        0.0075,     // 6 - GMID: Midpoint of sigmoid g filter curve
        33.33,      // 7 - GSLOPE: slope of sigmoid g filter curve
        10.0,       // 8 - TAUSHIFT: shift of learning curve
        0.00006);   // 9 - GSYN0: value of syn conductance g decays to

    InitVarSnippet::NormalClippedDelay::ParamValues gDist(
        0.0,    // 0 - mean
        0.0,    // 1 - sd
        0.0,     // 2 - min
        2.0); // 3 - max

    WeightUpdateModels::PiecewiseSTDP::VarValues stdp_ini(
        initVar<InitVarSnippet::NormalClipped>(gDist),    // 0 - g: synaptic conductance
        uninitialisedVar());                    // 1 - graw: raw synaptic conductance



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
        0.0,                                    // 0 - g
        0.0,                                    // 1 - PijStar
        std::numeric_limits<float>::lowest());  // 2 - lastUpdateTime

    BCPNNTwoTrace::PreVarValues bcpnnPreInit(
        0.0,    // 0 - ZiStar
        0.0);   // 1 - PiStar

    BCPNNTwoTrace::PostVarValues bcpnnPostInit(
        0.0,    // 0 - ZjStar
        0.0);   // 1 - PjStar

    // Exponential current parameters
    PostsynapticModels::ExpCond::ParamValues ps_p(
        1.0,    // 0 - tau_S: decay time constant for S [ms]
        -80.0); // 1 - Erev: Reversal potential


    // Create adex neuron
    model.addNeuronPopulation<NeuronModels::SpikeSource>("PreStim", 1, {}, {});
    model.addNeuronPopulation<NeuronModels::SpikeSource>("PostStim", 1, {}, {});
    auto *pre = model.addNeuronPopulation<SimpleAdEx>("Pre", 1, p_pyramidal, ini_pyramidal);
    auto *post = model.addNeuronPopulation<SimpleAdEx>("Post", 1, p_pyramidal, ini_pyramidal);
    
    // enable recording
    pre->setSpikeRecordingEnabled(true);
    post->setSpikeRecordingEnabled(true);

    model.addSynapsePopulation<WeightUpdateModels::StaticPulse, PostsynapticModels::ExpCond>(
            "PreStimToPre", SynapseMatrixType::SPARSE_GLOBALG, NO_DELAY,
            "PreStim", "Pre",
            {}, staticSynapseInit,
            ps_p, {},
            initConnectivity<InitSparseConnectivitySnippet::OneToOne>());
    model.addSynapsePopulation<WeightUpdateModels::StaticPulse, PostsynapticModels::ExpCond>(
            "PostStimToPost", SynapseMatrixType::SPARSE_GLOBALG, NO_DELAY,
            "PostStim", "Post",
            {}, staticSynapseInit,
            ps_p, {},
            initConnectivity<InitSparseConnectivitySnippet::OneToOne>());

    #ifdef stdp
    model.addSynapsePopulation<WeightUpdateModels::PiecewiseSTDP, PostsynapticModels::ExpCond>(
            "PreToPost", SynapseMatrixType::SPARSE_INDIVIDUALG, NO_DELAY,
            "Pre", "Post",
            stdp_p,  stdp_ini,
            ps_p, {},
            initConnectivity<InitSparseConnectivitySnippet::OneToOne>());
    #endif



    #ifdef bcpnn_simple
    model.addSynapsePopulation<BCPNNTwoTrace, PostsynapticModels::ExpCond>(
            "PreToPost", SynapseMatrixType::SPARSE_INDIVIDUALG, NO_DELAY,
            "Pre", "Post",
            bcpnnParams,  bcpnnInit, bcpnnPreInit, bcpnnPostInit,
            ps_p, {},
            initConnectivity<InitSparseConnectivitySnippet::OneToOne>());
    #endif
}