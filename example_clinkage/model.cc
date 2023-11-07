// Model definintion file tenHHRing.cc
#include "modelSpec.h"
void modelDefinition(ModelSpec &model)
{
    // definition of tenHHRing
    model.setDT(0.1);
    model.setName("example");
    NeuronModels::TraubMiles::ParamValues p(
        7.15,       // 0 - gNa: Na conductance in muS
        50.0,       // 1 - ENa: Na equi potential in mV
        1.43,       // 2 - gK: K conductance in muS
        -95.0,      // 3 - EK: K equi potential in mV
        0.02672,    // 4 - gl: leak conductance in muS
        -63.563,    // 5 - El: leak equi potential in mV
        0.143);     // 6 - Cmem: membr. capacity density in nF
    NeuronModels::TraubMiles::VarValues ini(
        -60.0,         // 0 - membrane potential V
        0.0529324,     // 1 - prob. for Na channel activation m
        0.3176767,     // 2 - prob. for not Na channel blocking h
        0.5961207);    // 3 - prob. for K channel activation n
    NeuronModels::SpikeSourceArray::VarValues stim_ini(
        uninitialisedVar(),     // 0 - startSpike indices
        uninitialisedVar());    // 1 - endSpike indices
    
    model.addNeuronPopulation<NeuronModels::TraubMiles>("Pop1", 1, p, ini);
    model.addNeuronPopulation<NeuronModels::SpikeSourceArray>("Stim", 1, {}, stim_ini);
    WeightUpdateModels::StaticPulse::VarValues s_ini(
         -0.2); // 0 - g: the synaptic conductance value
    PostsynapticModels::ExpCond::ParamValues ps_p(
        1.0,    // 0 - tau_S: decay time constant for S [ms]
        -80.0); // 1 - Erev: Reversal potential

    model.addSynapsePopulation<WeightUpdateModels::StaticPulse, PostsynapticModels::ExpCond>(
        "StimPop1", SynapseMatrixType::SPARSE_GLOBALG, NO_DELAY,
        "Stim", "Pop1",
        {}, s_ini,
        ps_p, {},
        initConnectivity<InitSparseConnectivitySnippet::OneToOne>());

    //record spikes
    //NeuronGroup *pop1 = model.findNeuronGroup("Pop1");
    //pop1->setSpikeRecordingEnabled(true); // seg fault
}