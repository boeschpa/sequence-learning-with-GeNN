// Model definintion file model.cc

#include "modelSpec.h"
#include <neuronModels.h>
#include "SimpleAdEx.h"
#include "AdEx.h"

#include <string>

// class Ring : public InitSparseConnectivitySnippet::Base
// {
// public:
//     DECLARE_SNIPPET(Ring, 0);
//     SET_ROW_BUILD_CODE(
//         "$(addSynapse, ($(id_pre) + 1) % $(num_post));\n"
//         "$(endRow);\n");
//     SET_MAX_ROW_LENGTH(1);
// };
// IMPLEMENT_SNIPPET(Ring);

void modelDefinition(ModelSpec &model)
{
    // definition of model
    model.setDT(0.1); // time step in ms
    model.setName("hypercolumn");
    int N_minicolumns = 10;
    int N_basket = 30;
    int N_pyramidal = 10;
    float wta_prob = 0.7;
    float lateral_prob = 0.25;

    SimpleAdEx::ParamValues p_pyramidal(
        0.28,         // 0 - Membrane capacitance [pF]
        14.0e-6,      // 1 - Membrane leak conductance [uS]
        -70.0,        // 2 - Resting membrane potential [mV]
        -70.0,        // 3 - Reset voltage [mV]
        -55.0,        // 4 - Spiking threshold [mV]
        3.0,          // 5 - spike upstroke slopefactor [mV]
        150.0,        // 6 - adaption time constant [ms]
        0.00001      // 7 - adatpion current per spike [nA]
    );

    SimpleAdEx::VarValues ini_pyramidal(
        -70.0,      // 0 - membrane potential V [mV]
        0.0        // 1 - Iw adaption current [pA]
    );
    
    SimpleAdEx::ParamValues p_basket( // no adaption
        0.28,         // 0 - Membrane capacitance [pF]
        14.0e-6,      // 1 - Membrane leak conductance [uS]
        -70.0,        // 2 - Resting membrane potential [mV]
        -70.0,        // 3 - Reset voltage [mV]
        -55.0,        // 4 - Spiking threshold [mV]
        3.0,          // 5 - spike upstroke slopefactor [mV]
        1.0,        // 6 - adaption time constant [ms]
        0.0      // 7 - adatpion current per spike [nA]
    );

    SimpleAdEx::VarValues ini_basket(
        -70.0,      // 0 - membrane potential V [mV]
        0.0        // 1 - Iw adaption current [pA]
    );

    WeightUpdateModels::StaticPulse::VarValues s_wta_ampa(
        -0.000); // 0 - conductance TODO
    PostsynapticModels::ExpCond::ParamValues ps_wta_ampa(
        5.0,    // 0 - tau_S: decay time constant for S [ms]
        -75.0); // 1 - Erev: Reversal potential TODO

    WeightUpdateModels::StaticPulse::VarValues s_wta_gaba(
        -0.000); // 0 - conductance TODO
    PostsynapticModels::ExpCond::ParamValues ps_wta_gaba(
        5.0,    // 0 - tau_S: decay time constant for S [ms]
        -75.0); // 1 - Erev: Reversal potential TODO

    WeightUpdateModels::StaticPulse::VarValues s_lateral_ampa(
        -0.000); // 0 - conductance TODO
    PostsynapticModels::ExpCond::ParamValues ps_lateral_ampa(
        5.0,    // 0 - tau_S: decay time constant for S [ms]
        -75.0); // 1 - Erev: Reversal potential TODO

    WeightUpdateModels::StaticPulse::VarValues s_lateral_nmda(
        -0.000); // 0 - conductance TODO
    PostsynapticModels::ExpCond::ParamValues ps_lateral_nmda(
        5.0,    // 0 - tau_S: decay time constant for S [ms]
        -75.0); // 1 - Erev: Reversal potential TODO

    



    // add minicolumn pop
    std::string minicolumn_name = "minicolumn_";
    for (int i = 0; i<N_minicolumns; i++){
        //std::string name = minicolumn_name + i;
        model.addNeuronPopulation<SimpleAdEx>(minicolumn_name+std::to_string(i), N_pyramidal, p_pyramidal, ini_pyramidal);
    }

    // add basket pop
    model.addNeuronPopulation<SimpleAdEx>("baskets", N_basket, p_basket, ini_basket);

    // add WTA connections
    InitSparseConnectivitySnippet::FixedProbabilityNoAutapse::ParamValues fixedProb(wta_prob);
    
    std::string wta_ampa_name = "wta_ampa_";
    std::string wta_gaba_name = "wta_gaba_";
    
    for (int i = 0; i<N_minicolumns; i++){
        // excitatory pyramidal-to-basket connections
        model.addSynapsePopulation<WeightUpdateModels::StaticPulse, PostsynapticModels::ExpCond>(
            wta_ampa_name+std::to_string(i), SynapseMatrixType::SPARSE_GLOBALG, 10,
            minicolumn_name+std::to_string(i), "baskets",
            {}, s_ini,
            ps_p, {},
            initConnectivity<InitSparseConnectivitySnippet::FixedProbabilityNoAutapse>(fixedProb));

        // inhibitory basekt-to-pyramidal connections
        model.addSynapsePopulation<WeightUpdateModels::StaticPulse, PostsynapticModels::ExpCond>(
            wta_ampa_name+std::to_string(i), SynapseMatrixType::SPARSE_GLOBALG, 10,
            minicolumn_name+std::to_string(i), "baskets",
            {}, s_ini,
            ps_p, {},
            initConnectivity<InitSparseConnectivitySnippet::FixedProbabilityNoAutapse>(fixedProb));

    }




    //model.addNeuronPopulation<AdEx>("Pop1", 10, p, ini);



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

