// Model definintion file

#include "modelSpec.h"
#include <neuronModels.h>

// class Ring : public InitSparseConnectivitySnippet::Base
// {
// public:
//     DECLARE_SNIPPET(Ring, 0);
//     SET_ROW_BUILD_CODE(
//         "$(addSynapse, ($(id_pre) + 1) % $(num_post));\n"
//         "$(endRow);\n");
//     SET_MAX_ROW_LENGTH(10);
// };
// IMPLEMENT_SNIPPET(Ring);

void modelDefinition(ModelSpec &model)
{
    // definition of tenLIFModel
    model.setDT(0.1); // time step in ms
    model.setName("denseLIF");

    NeuronModels::LIF::ParamValues p(
        0.28,        // 0 - Membrane capacitance (nF?)
        50.0,       // 1 - Membrane time constant [ms]
        -70.0,      // 2 - Resting membrane potential [mV]
        -64.0,       // 3 - Reset voltage [mV]
        -50.0,      // 4 - Spiking threshold [mV]
        -0.0,        // 5 - Offset current
        5.0        // 6 - refractory time constant tau refrac
    );

    NeuronModels::LIF::VarValues ini(
        -64.0,      // 0 - membrane potential V
        0.0        // 1 - refractory time constant ms
    );
    
    model.addNeuronPopulation<NeuronModels::LIF>("Pop1", 10, p, ini);

    // add synapses between neurons

    WeightUpdateModels::StaticPulse::VarValues s_ini(
        -0.01); // 0 - conductance
    PostsynapticModels::ExpCond::ParamValues ps_p(
        50.0,    // 0 - tau_S: decay time constant for S [ms]
        -80.0); // 1 - Erev: Reversal potential

    InitSparseConnectivitySnippet::FixedProbabilityNoAutapse::ParamValues fixedProb(0.3);

    model.addSynapsePopulation<WeightUpdateModels::StaticPulse, PostsynapticModels::ExpCond>(
        "Pop1self", SynapseMatrixType::SPARSE_GLOBALG, 10,
        "Pop1", "Pop1",
        {}, s_ini,
        ps_p, {},
        initConnectivity<InitSparseConnectivitySnippet::FixedProbabilityNoAutapse>(fixedProb));

    // add recursive connections

    WeightUpdateModels::StaticPulse::VarValues s_ini_recc(
        -0.1); // 0 - conductance
    PostsynapticModels::ExpCond::ParamValues ps_p_recc(
        50.0,    // 0 - tau_S: decay time constant for S [ms]
        -80.0); // 1 - Erev: Reversal potential

    model.addSynapsePopulation<WeightUpdateModels::StaticPulse, PostsynapticModels::ExpCond>(
        "Pop1selfrRecc", SynapseMatrixType::SPARSE_GLOBALG, 10,
        "Pop1", "Pop1",
        {}, s_ini_recc,
        ps_p_recc, {},
        initConnectivity<InitSparseConnectivitySnippet::OneToOne>());

    // add stimulation neuron and synapse

    NeuronModels::SpikeSourceArray::VarValues stim_ini(
        uninitialisedVar(),     // 0 - startSpike indices
        uninitialisedVar());    // 1 - endSpike indices
    
    model.addNeuronPopulation<NeuronModels::SpikeSourceArray>("Stim", 1, {}, stim_ini);
    model.addSynapsePopulation<WeightUpdateModels::StaticPulse, PostsynapticModels::ExpCond>(
        "StimPop1", SynapseMatrixType::SPARSE_GLOBALG, NO_DELAY,
        "Stim", "Pop1",
        {}, s_ini,
        ps_p, {},
        initConnectivity<InitSparseConnectivitySnippet::OneToOne>());
}


// run "genn-buildmodel.sh -c tenLIFRingModel.cc"
