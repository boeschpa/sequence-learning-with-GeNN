// Model definintion file tenHHRing.cc
#include "modelSpec.h"
#include "StaticPulseDendriticDelayStd.h"

// RING SNIPPET
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

// MODEL
void modelDefinition(ModelSpec &model)
{
    // NEURON POPULATIONS
    //  definition of tenHHRing
    model.setDT(0.1);
    model.setName("tenHHRing");
    NeuronModels::TraubMiles::ParamValues p(
        7.15,    // 0 - gNa: Na conductance in muS
        50.0,    // 1 - ENa: Na equi potential in mV
        1.43,    // 2 - gK: K conductance in muS
        -95.0,   // 3 - EK: K equi potential in mV
        0.02672, // 4 - gl: leak conductance in muS
        -63.563, // 5 - El: leak equi potential in mV
        0.143);  // 6 - Cmem: membr. capacity density in nF
    NeuronModels::TraubMiles::VarValues ini(
        -60.0,      // 0 - membrane potential V
        0.0529324,  // 1 - prob. for Na channel activation m
        0.3176767,  // 2 - prob. for not Na channel blocking h
        0.5961207); // 3 - prob. for K channel activation n
    NeuronModels::SpikeSourceArray::VarValues stim_ini(
        uninitialisedVar(),  // 0 - startSpike indices
        uninitialisedVar()); // 1 - endSpike indices
    model.addNeuronPopulation<NeuronModels::TraubMiles>("Pop1", 10, p, ini);
    model.addNeuronPopulation<NeuronModels::SpikeSourceArray>("Stim", 1, {}, stim_ini);

    // SYNAPSES
    // Build distribution for delay parameters
    InitVarSnippet::NormalClippedDelay::ParamValues dDist(
        10.0,    // 0 - mean
        0.0,    // 1 - sd
        0.0,     // 2 - min
        1000.0); // 3 - max

    InitVarSnippet::NormalClippedDelay::ParamValues xDist(
        1.0,    // 0 - mean
        0.0,    // 1 - sd
        0.0,     // 2 - min
        2.0); // 3 - max

    // Create weight parameters
    StaticPulseDendriticDelayStd::VarValues update_vars(
        -0.2,                                               // 0 - Wij (nA)
        initVar<InitVarSnippet::NormalClippedDelay>(dDist), // 1 - delay (ms)
        initVar<InitVarSnippet::NormalClipped>(xDist));     // 2 - STD depletion variable

    PostsynapticModels::ExpCond::ParamValues ps_p(
        1.0,    // 0 - tau_S: decay time constant for S [ms]
        -80.0); // 1 - Erev: Reversal potential

    StaticPulseDendriticDelayStd::ParamValues update_params(
        800.0, // 0 tau recovery time [ms]
        0.25); // 1 spike depletion fraction

    Ring::ParamValues ringParam(1);

    // model.addSynapsePopulation<WeightUpdateModels::StaticPulse, PostsynapticModels::ExpCond>(
    //     "Pop1self", SynapseMatrixType::SPARSE_GLOBALG, 100,
    //     "Pop1", "Pop1",
    //     {}, s_ini,
    //     ps_p, {},
    //     initConnectivity<Ring>(ringParam));
    auto *synPop = model.addSynapsePopulation<StaticPulseDendriticDelayStd, PostsynapticModels::ExpCond>(
        "StimPop1", SynapseMatrixType::SPARSE_INDIVIDUALG, NO_DELAY,
        "Stim", "Pop1",
        update_params, update_vars,
        ps_p, {},
        initConnectivity<InitSparseConnectivitySnippet::OneToOne>());

    // Set max dendritic delay and span type
    synPop->setMaxDendriticDelayTimesteps(std::rint(1000.0 / model.getDT()));

    // record spikes
    // NeuronGroup *pop1 = model.findNeuronGroup("Pop1");
    // pop1->setSpikeRecordingEnabled(true); // seg fault
}