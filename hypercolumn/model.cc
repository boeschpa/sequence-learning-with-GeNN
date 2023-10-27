// Model definintion file model.cc

#include "modelSpec.h"
#include <neuronModels.h>
#include "SimpleAdEx.h"
#include "AdEx.h"
#include "StaticPulseDendriticDelayStd.h"

#include <string>

void modelDefinition(ModelSpec &model)
{
    // definition of model
    model.setDT(0.1); // time step in ms
    const int N_minicolumns = 10;
    const int N_basket = 30;
    const int N_pyramidal = 10;
    const float wta_prob = 0.7;
    const float lateral_prob = 0.25;
    const int hyper_width = 2;
    const int hyper_height = 2;
    model.setName("hypercolumns_" + std::to_string(hyper_width) + "by" + std::to_string(hyper_height));

    ////////////////////////////
    /// PARAMETERS + INITIAL VALUES
    ////////////////////////////

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

    SimpleAdEx::ParamValues p_basket( // no adaption
        0.28,                         // 0 - Membrane capacitance [pF]
        14.0e-3,                      // 1 - Membrane leak conductance [uS]  14 nS
        -70.0,                        // 2 - Resting membrane potential [mV]
        -70.0,                        // 3 - Reset voltage [mV]
        -55.0,                        // 4 - Spiking threshold [mV]
        3.0,                          // 5 - spike upstroke slopefactor [mV]
        150.0,                        // 6 - adaption time constant [ms]
        0.0                           // 7 - adatpion current per spike [nA]
    );

    SimpleAdEx::VarValues ini_basket(
        -70.0, // 0 - membrane potential V [mV]
        0.0    // 1 - Iw adaption current [pA]
    );

    WeightUpdateModels::StaticPulse::VarValues s_wta_ampa(
        0.00602); // 0 - conductance 6.02 nS
    PostsynapticModels::ExpCond::ParamValues ps_wta_ampa(
        5.0,  // 0 - tau_S: decay time constant for S [ms]
        0.0); // 1 - Erev: Reversal potential AMPA

    WeightUpdateModels::StaticPulse::VarValues s_wta_gaba(
        0.00602); // 0 - conductance TODO
    PostsynapticModels::ExpCond::ParamValues ps_wta_gaba(
        5.0,    // 0 - tau_S: decay time constant for S [ms]
        -70.0); // 1 - Erev: Reversal potential GABA

    InitSparseConnectivitySnippet::FixedProbabilityNoAutapse::ParamValues wtaProb(wta_prob);

    float lateral_ampa_conductance = 0.00602; // 6.02 nS
    float lateral_nmda_conductance = 0.00122; // 1.22 nS

    StaticPulseDendriticDelayStd::VarValues update_vars_lateral_ampa(
        lateral_ampa_conductance, // 0 - conductance
        0.0,                      // 1 - delay in ms?
        1.0);                     // 2 - STD depletion variable
    StaticPulseDendriticDelayStd::ParamValues update_params_lateral_ampa(
        800.0, // 0 tau recovery time [ms]
        0.25); // 1 spike depletion fraction

    PostsynapticModels::ExpCond::ParamValues ps_lateral_ampa(
        5.0,  // 0 - tau_S: decay time constant for S [ms]
        0.0); // 1 - Erev: Reversal potential AMPA

    StaticPulseDendriticDelayStd::VarValues update_vars_lateral_nmda(
        lateral_nmda_conductance, // 0 - conductance
        0.0,                      // 1 - delay in ms?
        1.0);                     // 2 - STD depletion variable
    StaticPulseDendriticDelayStd::ParamValues update_params_lateral_nmda(
        800.0, // 0 tau recovery time [ms]
        0.25); // 1 spike depletion fraction

    PostsynapticModels::ExpCond::ParamValues ps_lateral_nmda(
        150.0, // 0 - tau_S: decay time constant for S [ms]
        0.0);  // 1 - Erev: Reversal potential NMDA

    InitSparseConnectivitySnippet::FixedProbabilityNoAutapse::ParamValues lateralProb(lateral_prob);

    ////////////////////////////
    /// NEURON POPULATIONS
    ////////////////////////////

    std::string minicolumn_basename = "M";
    std::string baskets_name = "baskets";
    std::string hypercolumn_basename = "H";
    std::string hypercolumn_name;

    // iterate over hypercolumns
    for (int m = 0; m < hyper_height; m++) // m = row number
    {
        for (int n = 0; n < hyper_width; n++) // n = column number
        {
            // hypercolumn_m_n
            hypercolumn_name = hypercolumn_basename + std::to_string(m) + "_" + std::to_string(n) + "_";

            // add minicolumns
            for (int i = 0; i < N_minicolumns; i++)
            {
                model.addNeuronPopulation<SimpleAdEx>(hypercolumn_name + minicolumn_basename + std::to_string(i), N_pyramidal, p_pyramidal, ini_pyramidal);
            }

            // add basket pop
            model.addNeuronPopulation<SimpleAdEx>(hypercolumn_name + baskets_name, N_basket, p_basket, ini_basket);
        }
    }

    ////////////////////////////
    /// Synapse POPULATIONS
    ////////////////////////////

    std::string wta_ampa_name = "wta_ampa";
    std::string wta_gaba_name = "wta_gaba";
    std::string lateral_ampa_name = "lateral_ampa";
    std::string lateral_nmda_name = "lateral_nmda";

    // iterate over hypercolumns
    for (int m = 0; m < hyper_height; m++) // m = row number
    {
        for (int n = 0; n < hyper_width; n++) // n = column number
        {
            // hypercolumn_m_n
            hypercolumn_name = hypercolumn_basename + std::to_string(m) + "_" + std::to_string(n) + "_";

            // add WTA connections
            for (int i = 0; i < N_minicolumns; i++)
            {
                // excitatory ampa pyramidal-to-basket connections
                model.addSynapsePopulation<WeightUpdateModels::StaticPulse, PostsynapticModels::ExpCond>(
                    hypercolumn_name + minicolumn_basename + std::to_string(i) + wta_ampa_name, SynapseMatrixType::SPARSE_GLOBALG, 10,
                    hypercolumn_name + minicolumn_basename + std::to_string(i), hypercolumn_name + baskets_name,
                    {}, s_wta_ampa,
                    ps_wta_ampa, {},
                    initConnectivity<InitSparseConnectivitySnippet::FixedProbability>(wtaProb));

                // inhibitory gaba basekt-to-pyramidal connections
                model.addSynapsePopulation<WeightUpdateModels::StaticPulse, PostsynapticModels::ExpCond>(
                    wta_gaba_name + hypercolumn_name + std::to_string(i), SynapseMatrixType::SPARSE_GLOBALG, 10,
                    hypercolumn_name + baskets_name, hypercolumn_name + minicolumn_basename + std::to_string(i),
                    {}, s_wta_gaba,
                    ps_wta_gaba, {},
                    initConnectivity<InitSparseConnectivitySnippet::FixedProbability>(wtaProb));
            }

            // todo add lateral synapses with right connectivity, get from matrix

            // AMPA (+positive) lateral connections within hypercolumn
            for (int i = 0; i < N_minicolumns; i++) // presynaptic index j
            {
                for (int j = 0; j < N_minicolumns; j++) // postsynaptic index i
                {
                    if (i == j)
                    {
                        // AMPA recurrent
                        model.addSynapsePopulation<StaticPulseDendriticDelayStd, PostsynapticModels::ExpCond>(
                            hypercolumn_name + minicolumn_basename + std::to_string(i) + "to" + std::to_string(j) + "_" + lateral_ampa_name, SynapseMatrixType::SPARSE_GLOBALG, 10,
                            hypercolumn_name + minicolumn_basename + std::to_string(j), hypercolumn_name + minicolumn_basename + std::to_string(i),
                            update_params_lateral_ampa, update_vars_lateral_ampa,
                            ps_lateral_ampa, {},
                            initConnectivity<InitSparseConnectivitySnippet::FixedProbabilityNoAutapse>(lateralProb));
                    }
                    else
                    {
                        // AMPA non-recurrent (not connected in static model)
                        // model.addSynapsePopulation<StaticPulseDendriticDelayStd, PostsynapticModels::ExpCond>(
                        //     hypercolumn_name + minicolumn_basename + std::to_string(i) + "to" + std::to_string(j) + "_" + lateral_ampa_name, SynapseMatrixType::SPARSE_GLOBALG, 10,
                        //      hypercolumn_name + minicolumn_basename + std::to_string(j), hypercolumn_name + minicolumn_basename + std::to_string(i),
                        //     update_params_lateral_ampa, update_vars_lateral_ampa,
                        //     ps_lateral_ampa, {},
                        //     initConnectivity<InitSparseConnectivitySnippet::FixedProbability>(lateralProb));
                    }
                }
            }

            // AMPA (+positive) lateral connections between hypercolumn
            for (int i = 0; i < N_minicolumns; i++) // presynaptic index j
            {
                for (int mp = 0; mp < hyper_height; mp++) // mp = postsynaptic hypercolum row number
                {
                    for (int np = 0; np < hyper_width; np++) // np = postsynaptic hypercolum column number
                    {
                        std::string to_hypercolumn = "to" + std::to_string(mp) + "_" + std::to_string(np) + "_";
                        if (m == mp && n == np)
                        {
                            for (int j = 0; j < N_minicolumns; j++) // postsynaptic index i
                            {
                                if (i == j)
                                {
                                    // AMPA recurrent
                                    model.addSynapsePopulation<StaticPulseDendriticDelayStd, PostsynapticModels::ExpCond>(
                                        hypercolumn_name + to_hypercolumn + minicolumn_basename + std::to_string(i) + "to" + std::to_string(j) + "_" + lateral_ampa_name, SynapseMatrixType::SPARSE_GLOBALG, 10,
                                        hypercolumn_name + minicolumn_basename + std::to_string(j), hypercolumn_name + minicolumn_basename + std::to_string(i),
                                        update_params_lateral_ampa, update_vars_lateral_ampa,
                                        ps_lateral_ampa, {},
                                        initConnectivity<InitSparseConnectivitySnippet::FixedProbability>(lateralProb));
                                }
                                else
                                {
                                    // AMPA non-recurrent (not connected in static model)
                                    // model.addSynapsePopulation<StaticPulseDendriticDelayStd, PostsynapticModels::ExpCond>(
                                    //     hypercolumn_name + minicolumn_basename + std::to_string(i) + "to" + std::to_string(j) + "_" + lateral_ampa_name, SynapseMatrixType::SPARSE_GLOBALG, 10,
                                    //      hypercolumn_name + minicolumn_basename + std::to_string(j), hypercolumn_name + minicolumn_basename + std::to_string(i),
                                    //     update_params_lateral_ampa, update_vars_lateral_ampa,
                                    //     ps_lateral_ampa, {},
                                    //     initConnectivity<InitSparseConnectivitySnippet::FixedProbability>(lateralProb));
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    // todo input neurons

    // todo loop over hypercolumns, build each independently,
    //  connect matching minicolumns with positive conductance, rest with 0 or negative

    // then loop again and connect all minicolumn-populations with calculated (euclidian) delay
}

// // AMPA positive lateral connections
//             for (int j = 0; j < N_minicolumns; j++)
//             { // later_todo iterate N_hypercolumns
//                 if (i == j)
//                 {

//                     // AMPA recurrent
//                     model.addSynapsePopulation<StaticPulseDendriticDelayStd, PostsynapticModels::ExpCond>(
//                         lateral_ampa_name + hypercolumn_name + std::to_string(j) + "_" + std::to_string(i), SynapseMatrixType::SPARSE_GLOBALG, 10,
//                         minicolumn_name + hypercolumn_name + std::to_string(j), minicolumn_name + hypercolumn_name + std::to_string(i),
//                         update_params_lateral_ampa, update_vars_lateral_ampa,
//                         ps_lateral_ampa, {},
//                         initConnectivity<InitSparseConnectivitySnippet::FixedProbabilityNoAutapse>(lateralProb));
//                     // NMDA recurrent
//                     model.addSynapsePopulation<StaticPulseDendriticDelayStd, PostsynapticModels::ExpCond>(
//                         lateral_nmda_name + hypercolumn_name + std::to_string(j) + "_" + std::to_string(i), SynapseMatrixType::SPARSE_GLOBALG, 10,
//                         minicolumn_name + hypercolumn_name + std::to_string(i), minicolumn_name + hypercolumn_name + std::to_string(i),
//                         update_params_lateral_nmda, update_vars_lateral_nmda,
//                         ps_lateral_nmda, {},
//                         initConnectivity<InitSparseConnectivitySnippet::FixedProbabilityNoAutapse>(lateralProb));
//                 }
//                 else
//                 {
//                     // AMPA non-recurrent
//                     model.addSynapsePopulation<StaticPulseDendriticDelayStd, PostsynapticModels::ExpCond>(
//                         lateral_ampa_name + hypercolumn_name + std::to_string(j) + "_" + std::to_string(i), SynapseMatrixType::SPARSE_GLOBALG, 10,
//                         minicolumn_name + hypercolumn_name + std::to_string(i), minicolumn_name + hypercolumn_name + std::to_string(i),
//                         update_params_lateral_ampa, update_vars_lateral_ampa,
//                         ps_lateral_ampa, {},
//                         initConnectivity<InitSparseConnectivitySnippet::FixedProbability>(lateralProb));
//                     // NMDA non-recurrent
//                     model.addSynapsePopulation<StaticPulseDendriticDelayStd, PostsynapticModels::ExpCond>(
//                         lateral_nmda_name + hypercolumn_name + std::to_string(j) + "_" + std::to_string(i), SynapseMatrixType::SPARSE_GLOBALG, 10,
//                         minicolumn_name + hypercolumn_name + std::to_string(i), minicolumn_name + hypercolumn_name + std::to_string(j),
//                         update_params_lateral_nmda, update_vars_lateral_nmda,
//                         ps_lateral_nmda, {},
//                         initConnectivity<InitSparseConnectivitySnippet::FixedProbability>(lateralProb));
//                 }
//             }