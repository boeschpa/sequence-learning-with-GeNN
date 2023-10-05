// Model definintion file model.cc

#include "modelSpec.h"
#include <neuronModels.h>
//#include "SimpleAdEx.h"
#include "AdEx.h"

class Ring : public InitSparseConnectivitySnippet::Base
{
public:
    DECLARE_SNIPPET(Ring, 0);
    SET_ROW_BUILD_CODE(
        "$(addSynapse, ($(id_pre) + 1) % $(num_post));\n"
        "$(endRow);\n");
    SET_MAX_ROW_LENGTH(1);
};
IMPLEMENT_SNIPPET(Ring);

void modelDefinition(ModelSpec &model)
{
    // definition of model
    model.setDT(0.1); // time step in ms
    model.setName("ringAdEx");

    AdEx::ParamValues p(
        0.28,         // 0 - Membrane capacitance [pF]
        14.0e-6,      // 1 - Membrane leak conductance [uS]
        -70.0,        // 2 - Resting membrane potential [mV]
        -70.0,        // 3 - Reset voltage [mV]
        -55.0,        // 4 - Spiking threshold [mV]
        3.0,          // 5 - spike upstroke slopefactor [mV]
        150.0,        // 6 - adaption time constant [ms]
        0.00001,         // 7 - adatpion current per spike [nA]
        0.000         // 8 - adaption coupling parameter a
    );

    AdEx::VarValues ini(
        -70.0,      // 0 - membrane potential V [mV]
        0.0        // 1 - Iw adaption current [pA]
    );
    
    model.addNeuronPopulation<AdEx>("Pop1", 10, p, ini);

    WeightUpdateModels::StaticPulse::VarValues s_ini(
        -0.000); // 0 - conductance
    PostsynapticModels::ExpCond::ParamValues ps_p(
        5.0,    // 0 - tau_S: decay time constant for S [ms]
        -75.0); // 1 - Erev: Reversal potential

    model.addSynapsePopulation<WeightUpdateModels::StaticPulse, PostsynapticModels::ExpCond>(
        "Pop1self", SynapseMatrixType::SPARSE_GLOBALG, 10,
        "Pop1", "Pop1",
        {}, s_ini,
        ps_p, {},
        initConnectivity<Ring>());

    NeuronModels::SpikeSourceArray::VarValues stim_ini(
        uninitialisedVar(),     // 0 - startSpike indices
        uninitialisedVar());    // 1 - endSpike indices
    
    model.addNeuronPopulation<NeuronModels::SpikeSourceArray>("Stim", 1, {}, stim_ini);

    WeightUpdateModels::StaticPulse::VarValues s_stimsyn_ini(
        -0.00006); // 0 - conductance
    PostsynapticModels::ExpCond::ParamValues ps_stimsyn_p(
        10.0,    // 0 - tau_S: decay time constant for S [ms]
        -75.0); // 1 - Erev: Reversal potential

    model.addSynapsePopulation<WeightUpdateModels::StaticPulse, PostsynapticModels::ExpCond>(
        "StimPop1", SynapseMatrixType::SPARSE_GLOBALG, NO_DELAY,
        "Stim", "Pop1",
        {}, s_stimsyn_ini,
        ps_stimsyn_p, {},
        initConnectivity<InitSparseConnectivitySnippet::OneToOne>());
}

