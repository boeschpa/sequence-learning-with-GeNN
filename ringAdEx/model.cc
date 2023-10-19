// Model definintion file model.cc

#include "modelSpec.h"
#include <neuronModels.h>
//#include "SimpleAdEx.h"
#include "AdEx.h"

class Ring : public InitSparseConnectivitySnippet::Base
{
public:
    DECLARE_SNIPPET(Ring, 1);
    SET_ROW_BUILD_STATE_VARS({{"offset", "unsigned int", 1}});
    SET_ROW_BUILD_CODE(
        // "if ($(id_pre)==5) {\n"
        // "   $(endRow);\n"
        // "}\n"
        "unsigned int target = ($(id_pre) + offset) % $(num_post);\n"
        "$(addSynapse, target);\n"
        "offset++;\n"
        "if(offset > (unsigned int)$(numNeighbours)) {\n"
        "   $(endRow);\n"
        "}\n");
    SET_PARAM_NAMES({"numNeighbours"});
    SET_CALC_MAX_ROW_LENGTH_FUNC(
        [](unsigned int numPre, unsigned int numPost, const std::vector<double> &pars)
        {
            return (unsigned int)pars[0];
        });
    SET_CALC_MAX_COL_LENGTH_FUNC(
        [](unsigned int numPre, unsigned int numPost, const std::vector<double> &pars)
        {
            return (unsigned int)pars[0];
        });
};
IMPLEMENT_SNIPPET(Ring);

void modelDefinition(ModelSpec &model)
{
    // definition of model
    model.setDT(0.1); // time step in ms
    model.setName("ringAdEx");
    int N = 20; // n neurons

    AdEx::ParamValues p(
        0.28,         // 0 - Membrane capacitance nF!
        14.0e-3,      // 1 - Membrane leak conductance [uS]
        -70.0,        // 2 - Resting membrane potential [mV]
        -70.0,        // 3 - Reset voltage [mV]
        -55.0,        // 4 - Spiking threshold [mV]
        3.0,          // 5 - spike upstroke slopefactor [mV]
        150.0,        // 6 - adaption time constant [ms]
        0.150,         // 7 - adatpion current per spike [nA]
        0.000         // 8 - adaption coupling parameter a  [uS]
    );

    AdEx::VarValues ini(
        -70.0,      // 0 - membrane potential V [mV]
        0.0        // 1 - Iw adaption current [nA]
    );
    
    model.addNeuronPopulation<AdEx>("Pop1", N, p, ini);

    WeightUpdateModels::StaticPulse::VarValues s_ini(
        0.04); // 0 - conductance [uS] nS??
    PostsynapticModels::ExpCond::ParamValues ps_p(
        5.0,    // 0 - tau_S: decay time constant for S [ms]
        0.0); // 1 - Erev: Reversal potential  [mV]  AMPA
        
    Ring::ParamValues ringParam(1);

    model.addSynapsePopulation<WeightUpdateModels::StaticPulse, PostsynapticModels::ExpCond>(
        "Pop1self", SynapseMatrixType::SPARSE_GLOBALG, 100,
        "Pop1", "Pop1",
        {}, s_ini,
        ps_p, {},
        initConnectivity<Ring>(ringParam));

    NeuronModels::SpikeSourceArray::VarValues stim_ini(
        uninitialisedVar(),     // 0 - startSpike indices
        uninitialisedVar());    // 1 - endSpike indices
    
    model.addNeuronPopulation<NeuronModels::SpikeSourceArray>("Stim", 1, {}, stim_ini);

    WeightUpdateModels::StaticPulse::VarValues s_stimsyn_ini(
        0.06); // 0 - conductance
    PostsynapticModels::ExpCond::ParamValues ps_stimsyn_p(
        100.0,    // 0 - tau_S: decay time constant for S [ms]
        0.0); // 1 - Erev: Reversal potential

    model.addSynapsePopulation<WeightUpdateModels::StaticPulse, PostsynapticModels::ExpCond>(
        "StimPop1", SynapseMatrixType::SPARSE_GLOBALG, NO_DELAY,
        "Stim", "Pop1",
        {}, s_stimsyn_ini,
        ps_stimsyn_p, {},
        initConnectivity<InitSparseConnectivitySnippet::OneToOne>());
}

