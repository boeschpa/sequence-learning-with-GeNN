// Model definintion file model.cc

#include "modelSpec.h"
#include <neuronModels.h>
#include "SimpleAdEx.h"
#include "AdEx.h"
#include "StaticPulseDendriticDelayStd.h"

#include <string>
#include <random>


void modelDefinition(ModelSpec &model)
{
    // definition of model
    model.setDT(0.1); // time step in ms
    std::string hypercolumn_name ="hypercolumn_1_";
    model.setName(hypercolumn_name);
    const int N_minicolumns = 10;
    const int N_basket = 30;
    const int N_pyramidal = 10;
    const float wta_prob = 0.7;
    const float lateral_prob = 0.25;

    SimpleAdEx::ParamValues p_pyramidal(
        0.28,         // 0 - Membrane capacitance [pF] 280 pf
        14.0e-3,      // 1 - Membrane leak conductance [uS] 14 ns
        -70.0,        // 2 - Resting membrane potential [mV]
        -70.0,        // 3 - Reset voltage [mV]
        -55.0,        // 4 - Spiking threshold [mV]
        3.0,          // 5 - spike upstroke slopefactor [mV]
        150.0,        // 6 - adaption time constant [ms]
        0.150      // 7 - adatpion current per spike [nA]  (150 pA)
    );

    SimpleAdEx::VarValues ini_pyramidal(
        -70.0,      // 0 - membrane potential V [mV]
        0.0        // 1 - Iw adaption current [pA]
    );
    
    SimpleAdEx::ParamValues p_basket( // no adaption
        0.28,         // 0 - Membrane capacitance [pF]
        14.0e-3,      // 1 - Membrane leak conductance [uS]  14 nS
        -70.0,        // 2 - Resting membrane potential [mV]
        -70.0,        // 3 - Reset voltage [mV]
        -55.0,        // 4 - Spiking threshold [mV]
        3.0,          // 5 - spike upstroke slopefactor [mV]
        150.0,        // 6 - adaption time constant [ms]
        0.0      // 7 - adatpion current per spike [nA]
    );

    SimpleAdEx::VarValues ini_basket(
        -70.0,      // 0 - membrane potential V [mV]
        0.0        // 1 - Iw adaption current [pA]
    );

    WeightUpdateModels::StaticPulse::VarValues s_wta_ampa(
        0.00602); // 0 - conductance 6.02 nS
    PostsynapticModels::ExpCond::ParamValues ps_wta_ampa(
        5.0,    // 0 - tau_S: decay time constant for S [ms]
        0.0); // 1 - Erev: Reversal potential AMPA

    WeightUpdateModels::StaticPulse::VarValues s_wta_gaba(
        0.006); // 0 - conductance TODO
    PostsynapticModels::ExpCond::ParamValues ps_wta_gaba(
        5.0,    // 0 - tau_S: decay time constant for S [ms]
        -70.0); // 1 - Erev: Reversal potential GABA


    float lateral_ampa_conductance = 0.00602; // 6.02 nS
    float lateral_nmda_conductance = 0.00122;  // 1.22 nS

    StaticPulseDendriticDelayStd::VarValues update_vars_lateral_ampa(
        lateral_ampa_conductance, // 0 - conductance TODO
        0.0, // 1 - delay in ms?
        1.0); // 2 - STD depletion variable
    StaticPulseDendriticDelayStd::ParamValues update_params_lateral_ampa(
        800.0, // 0 tau recovery time [ms]
        0.25); // 1 spike depletion fraction
    
    PostsynapticModels::ExpCond::ParamValues ps_lateral_ampa(
        5.0,    // 0 - tau_S: decay time constant for S [ms]
        0.0); // 1 - Erev: Reversal potential AMPA

    StaticPulseDendriticDelayStd::VarValues update_vars_lateral_nmda(
        lateral_nmda_conductance, // 0 - conductance TODO
        0.0, // 1 - delay in ms?
        1.0); // 2 - STD depletion variable
    StaticPulseDendriticDelayStd::ParamValues update_params_lateral_nmda(
        800.0, // 0 tau recovery time [ms]
        0.25); // 1 spike depletion fraction

    PostsynapticModels::ExpCond::ParamValues ps_lateral_nmda(
        150.0,    // 0 - tau_S: decay time constant for S [ms]
        0.0); // 1 - Erev: Reversal potential NMDA

    // add minicolumn pop
    std::string minicolumn_name = "minicolumn_";
    for (int i = 0; i<N_minicolumns; i++){
        //std::string name = minicolumn_name + i;
        model.addNeuronPopulation<SimpleAdEx>(minicolumn_name+hypercolumn_name+std::to_string(i), N_pyramidal, p_pyramidal, ini_pyramidal);
    }

    // add basket pop
    std::string baskets_name = "baskets_";
    model.addNeuronPopulation<SimpleAdEx>(baskets_name+hypercolumn_name, N_basket, p_basket, ini_basket);

    // add WTA connections
    InitSparseConnectivitySnippet::FixedProbabilityNoAutapse::ParamValues wtaProb(wta_prob);
    
    std::string wta_ampa_name = "wta_ampa_";
    std::string wta_gaba_name = "wta_gaba_";

    // std::random_device rd;
    // std::bernoulli_distribution bern_dist(wta_prob);
    // std::mt19937 engine(rd()); // Mersenne twister MT19937

    //todo initialize connectivity in simulation.cc

    for (int i = 0; i<N_minicolumns; i++){
        // bool wta_connect[N_pyramidal][N_basket] = {false};
        // for (int m = 0; m<N_pyramidal; m++){
        //     for (int n = 0; n<N_basket; n++){
        //         wta_connect[m][n] = bern_dist(engine);
        //     }
        // }

        // excitatory ampa pyramidal-to-basket connections
        model.addSynapsePopulation<WeightUpdateModels::StaticPulse, PostsynapticModels::ExpCond>(
            wta_ampa_name+hypercolumn_name+std::to_string(i), SynapseMatrixType::SPARSE_GLOBALG, 10,
            minicolumn_name+hypercolumn_name+std::to_string(i), baskets_name+hypercolumn_name,
            {}, s_wta_ampa,
            ps_wta_ampa, {},
            initConnectivity<InitSparseConnectivitySnippet::FixedProbability>(wtaProb));  // todo same connectivity for both directions

        // inhibitory gaba basekt-to-pyramidal connections
        model.addSynapsePopulation<WeightUpdateModels::StaticPulse, PostsynapticModels::ExpCond>(
            wta_gaba_name+hypercolumn_name+std::to_string(i), SynapseMatrixType::SPARSE_GLOBALG, 10,
            baskets_name+hypercolumn_name, minicolumn_name+hypercolumn_name+std::to_string(i), 
            {}, s_wta_gaba,
            ps_wta_gaba, {},
            initConnectivity<InitSparseConnectivitySnippet::FixedProbability>(wtaProb));
    }

    // add lateral connections
    InitSparseConnectivitySnippet::FixedProbabilityNoAutapse::ParamValues lateralProb(lateral_prob);
    
    std::string lateral_ampa_name = "lateral_ampa_";
    std::string lateral_nmda_name = "lateral_nmda_";


    for (int i = 0; i<N_minicolumns; i++){
        // AMPA and NMDA lateral connections
        for (int j = 0; j<N_minicolumns; j++){ // later_todo iterate N_hypercolumns 
            if (i==j){

                // AMPA recurrent
                model.addSynapsePopulation<StaticPulseDendriticDelayStd, PostsynapticModels::ExpCond>(
                    lateral_ampa_name+hypercolumn_name+std::to_string(j)+"_"+std::to_string(i), SynapseMatrixType::SPARSE_GLOBALG, 10,
                    minicolumn_name+hypercolumn_name+std::to_string(j), minicolumn_name+hypercolumn_name+std::to_string(i),
                    update_params_lateral_ampa, update_vars_lateral_ampa,
                    ps_lateral_ampa, {},
                    initConnectivity<InitSparseConnectivitySnippet::FixedProbabilityNoAutapse>(lateralProb));
                // NMDA recurrent
                model.addSynapsePopulation<StaticPulseDendriticDelayStd, PostsynapticModels::ExpCond>(
                    lateral_nmda_name+hypercolumn_name+std::to_string(j)+"_"+std::to_string(i), SynapseMatrixType::SPARSE_GLOBALG, 10,
                    minicolumn_name+hypercolumn_name+std::to_string(i), minicolumn_name+hypercolumn_name+std::to_string(i),
                    update_params_lateral_nmda, update_vars_lateral_nmda,
                    ps_lateral_nmda, {},
                    initConnectivity<InitSparseConnectivitySnippet::FixedProbabilityNoAutapse>(lateralProb));
            }
            else {
                // AMPA non-recurrent
                model.addSynapsePopulation<StaticPulseDendriticDelayStd, PostsynapticModels::ExpCond>(
                    lateral_ampa_name+hypercolumn_name+std::to_string(j)+"_"+std::to_string(i), SynapseMatrixType::SPARSE_GLOBALG, 10,
                    minicolumn_name+hypercolumn_name+std::to_string(i), minicolumn_name+hypercolumn_name+std::to_string(i),
                    update_params_lateral_ampa, update_vars_lateral_ampa,
                    ps_lateral_ampa, {},
                    initConnectivity<InitSparseConnectivitySnippet::FixedProbability>(lateralProb));
                // NMDA non-recurrent
                model.addSynapsePopulation<StaticPulseDendriticDelayStd, PostsynapticModels::ExpCond>(
                    lateral_nmda_name+hypercolumn_name+std::to_string(j)+"_"+std::to_string(i), SynapseMatrixType::SPARSE_GLOBALG, 10,
                    minicolumn_name+hypercolumn_name+std::to_string(i), minicolumn_name+hypercolumn_name+std::to_string(j),
                    update_params_lateral_nmda, update_vars_lateral_nmda,
                    ps_lateral_nmda, {},
                    initConnectivity<InitSparseConnectivitySnippet::FixedProbability>(lateralProb));
            }
        }
    }


    // todo input neurons

    // todo loop over hypercolumns, build each independently, 
    // then loop again and connect all minicolumn-populations with calculated (euclidian) delay
}

