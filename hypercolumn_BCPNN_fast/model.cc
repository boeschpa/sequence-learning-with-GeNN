// Model definintion file model.cc

#include "modelSpec.h"
#include <neuronModels.h>
#include "SimpleAdEx.h"
#include "StaticPulseDendriticDelayStd.h"
#include "model_param.h"
#include "bcpnn.h"

#include <string>

void modelDefinition(ModelSpec &model)
{
    // definition of model
    const std::string model_name = "hypercolumn";
    model.setDT(time_step);    // time step in ms
    model.setName(model_name); //_" + std::to_string(hyper_width) + "by" + std::to_string(hyper_height));

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
        15.0,    // 6 - adaption time constant [ms]
        0.15,    // 7 - adatpion current per spike [nA]  (150 pA)
        5.0,     // TauZ
        5000.0,  // TauP
        epsilon,   // epsilon
        1.0,     // deltaT
        fMax,   // fMax
        0.05     // biasGainBase 50 pA
    );

    SimpleAdEx::VarValues ini_pyramidal(
        -70.0, // 0 - membrane potential V [mV]
        0.0,    // 1 - Iw adaption current [pA]
        0.0,   // Zj
        0.01,  // Pj
        0.0,   // Ib
        1.0,   // kappa
        0.0    // biasGain
    );

    SimpleAdEx::ParamValues p_basket( // no adaption
        0.28,                         // 0 - Membrane capacitance [pF]
        14.0e-3,                      // 1 - Membrane leak conductance [uS]  14 nS
        -70.0,                        // 2 - Resting membrane potential [mV]
        -70.0,                        // 3 - Reset voltage [mV]
        -55.0,                        // 4 - Spiking threshold [mV]
        3.0,                          // 5 - spike upstroke slopefactor [mV]
        150.0,                        // 6 - adaption time constant [ms]
        0.0,                          // 7 - adatpion current per spike [nA]  (150 pA)
        5.0,                          // TauZ
        5000.0,                       // TauP
        epsilon,                        // epsilon
        1.0,                          // deltaT
        fMax,                        // fMax
        0.0                           // biasGainBase 50 pA
    );

    SimpleAdEx::VarValues ini_basket(
        -70.0, // 0 - membrane potential V [mV]
        0.0,    // 1 - Iw adaption current [pA]
        0.0,   // Zj
        0.01,  // Pj
        0.0,   // Ib
        0.0,   // kappa
        0.0    // biasGain
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

    float lateral_ampa_conductance = 0.001;  // 0.00602; // 6.02 nS
    float lateral_nmda_conductance = 0.0005; // 0.00122; // 1.22 nS

    // delay parameters
    float maxDelay = 20.0;
    float d_norm = 0.75;               // mm
    float d_V = 0.2;                   // mm/ms
    float d_mean = 1.0 + d_norm / d_V; // ms

    InitVarSnippet::NormalClippedDelay::ParamValues dDist(
        d_mean,       // 0 - mean
        d_mean * 0.1, // 1 - sd
        0.0,          // 2 - min
        maxDelay);    // 3 - max

    BCPNN::ParamValues update_params_lateral_ampa(
        5.0,    // 0 - Time constant of presynaptic primary trace (ms)
        5.0,    // 1 - Time constant of postsynaptic primary trace (ms)
        5000.0, // 2 - Time constant of probability trace
        fMax,  // 3 - Maximum firing frequency (Hz)
        1.0,    // 5 - spike duration (ms)
        epsilon,  // 6 - epsilon
        800.0,  // 7 - short term depression time constant
        0.1);   // 7 - depletion fraction

    PostsynapticModels::ExpCond::ParamValues ps_lateral_ampa(
        5.0,  // 0 - tau_S: decay time constant for S [ms]
        0.0); // 1 - Erev: Reversal potential AMPA

    BCPNN::ParamValues update_params_lateral_nmda(
        150.0,  // 0 - Time constant of presynaptic primary trace (ms)
        5.0,    // 1 - Time constant of postsynaptic primary trace (ms)
        5000.0, // 2 - Time constant of probability trace
        fMax,  // 3 - Maximum firing frequency (Hz)                todo set right
        1.0,    // 5 - spike duration (ms)
        epsilon,  // 6 - epsilon
        800.0,  // 7 - short term depression time constant
        0.25);  // 7 - depletion fraction

    PostsynapticModels::ExpCond::ParamValues ps_lateral_nmda(
        150.0, // 0 - tau_S: decay time constant for S [ms]
        0.0);  // 1 - Erev: Reversal potential NMDA

    InitSparseConnectivitySnippet::FixedProbabilityNoAutapse::ParamValues lateralProb(lateral_prob);

    // input neurons local parameters
    NeuronModels::Poisson::ParamValues p_stim(
        0.1,    // Refractory period (ms)
        0.1,    // duration of spike (ms)
        0.0,    // Membrane potential at spike (mV)
        -60.0); // Membrane potential at rest (mV)
    // input neuron initial values
    NeuronModels::Poisson::VarValues stim_ini(
        0.0,  // V - Membrane potential
        0.0); // SpikeTime - Time at which the neuron spiked for the last time

    // input synapse vars and params
    WeightUpdateModels::StaticPulse::VarValues s_input_ampa(
        0.01); // 0 - conductance 5 nS
    PostsynapticModels::ExpCond::ParamValues ps_input_ampa(
        5.0,  // 0 - tau_S: decay time constant for S [ms]
        0.0); // 1 - Erev: Reversal potential AMPA

    ////////////////////////////
    /// NEURON POPULATIONS
    ////////////////////////////

    std::string hypercolumn_name;
    std::string hypercolumn_name_post;

    // iterate over hypercolumns
    for (int m = 0; m < hyper_height; m++) // m = row number
    {
        for (int n = 0; n < hyper_width; n++) // n = column number
        {
            // hypercolumn_m_n
            hypercolumn_name = hypercolumn_basename + std::to_string(m) + "_" + std::to_string(n);

            // add minicolumns
            auto *pop = model.addNeuronPopulation<SimpleAdEx>(hypercolumn_name, N_pyramidal * N_minicolumns, p_pyramidal, ini_pyramidal);
            pop->setSpikeRecordingEnabled(true);

            // add basket pop
            auto *pop_basket = model.addNeuronPopulation<SimpleAdEx>(hypercolumn_name + baskets_name, N_basket, p_basket, ini_basket);
            pop_basket->setSpikeRecordingEnabled(true);

            // add input neurons
            auto *pop_input = model.addNeuronPopulation<NeuronModels::Poisson>(hypercolumn_name + input_basename, N_pyramidal * N_minicolumns, p_stim, stim_ini);
            pop_input->setSpikeRecordingEnabled(true);
        }
    }

    ////////////////////////////
    /// Synapse POPULATIONS
    ////////////////////////////

    // iterate over hypercolumns
    for (int m = 0; m < hyper_height; m++) // m = row number
    {
        for (int n = 0; n < hyper_width; n++) // n = column number
        {
            // hypercolumn_m_n
            hypercolumn_name = hypercolumn_basename + std::to_string(m) + "_" + std::to_string(n);

            // add WTA connections
            // excitatory ampa pyramidal-to-basket connections
            model.addSynapsePopulation<WeightUpdateModels::StaticPulse, PostsynapticModels::ExpCond>(
                hypercolumn_name + wta_ampa_name, SynapseMatrixType::SPARSE_GLOBALG, 1,
                hypercolumn_name, hypercolumn_name + baskets_name,
                {}, s_wta_ampa,
                ps_wta_ampa, {},
                initConnectivity<InitSparseConnectivitySnippet::FixedProbability>(wtaProb));

            // inhibitory gaba basekt-to-pyramidal connections
            model.addSynapsePopulation<WeightUpdateModels::StaticPulse, PostsynapticModels::ExpCond>(
                hypercolumn_name + wta_gaba_name, SynapseMatrixType::SPARSE_GLOBALG, 1,
                hypercolumn_name + baskets_name, hypercolumn_name,
                {}, s_wta_gaba,
                ps_wta_gaba, {},
                initConnectivity<InitSparseConnectivitySnippet::FixedProbability>(wtaProb));

            // Lateral connections between hypercolumn (and within)
            for (int mp = 0; mp < hyper_height; mp++) // mp = postsynaptic hypercolum row number
            {
                for (int np = 0; np < hyper_width; np++) // np = postsynaptic hypercolum column number
                {
                    hypercolumn_name_post = hypercolumn_basename + std::to_string(mp) + "_" + std::to_string(np);
                    // calculate distance dependent delay
                    d_mean = 1 + (d_norm * sqrt((m - mp) * (m - mp) + (n - np) * (n - np))) / d_V; // ms
                    InitVarSnippet::NormalClippedDelay::ParamValues dDist(
                        d_mean,       // 0 - mean
                        d_mean * 0.1, // 1 - sd
                        0.0,          // 2 - min
                        maxDelay);    // 3 - max

                    BCPNN::VarValues update_vars_lateral_nmda(
                        0.0,                                                // 0 - g
                        0.00,                                               // 1 - PijStar
                        0.0,                                                // Zi
                        0.00,                                               // Pi
                        0.0,                                                // Zj
                        0.00,                                               // Pj
                        lateral_nmda_conductance,                           // w_gain_base
                        0.0,                                                // w_gain
                        0.0,                                                // kappa
                        initVar<InitVarSnippet::NormalClippedDelay>(dDist), // delay d
                        1.0);                                               // x

                    BCPNN::VarValues update_vars_lateral_ampa(
                        0.0,                                                // 0 - g
                        0.00,                                               // 1 - PijStar
                        0.0,                                                // Zi
                        0.00,                                               // Pi
                        0.0,                                                // Zj
                        0.00,                                               // Pj
                        lateral_ampa_conductance,                           // w_gain_base
                        0.0,                                                // w_gain
                        0.0,                                                // kappa
                        initVar<InitVarSnippet::NormalClippedDelay>(dDist), // delay d
                        1.0);                                               // x

                    if (m != mp || n != np) // if between hypercolumns
                    {
                        // AMPA recurrent
                        auto *synPop = model.addSynapsePopulation<BCPNN, PostsynapticModels::ExpCond>(
                            hypercolumn_name + "_to_" + hypercolumn_name_post + lateral_ampa_name, SynapseMatrixType::SPARSE_INDIVIDUALG, 10,
                            hypercolumn_name, hypercolumn_name_post,
                            update_params_lateral_ampa, update_vars_lateral_ampa,
                            ps_lateral_ampa, {},
                            initConnectivity<InitSparseConnectivitySnippet::FixedProbability>(lateralProb));
                        synPop->setMaxDendriticDelayTimesteps(std::rint(maxDelay / model.getDT()));
                    }
                    else
                    {
                        // AMPA recurrent (no autapse)
                        auto *synPop = model.addSynapsePopulation<BCPNN, PostsynapticModels::ExpCond>(
                            hypercolumn_name + "_to_" + hypercolumn_name_post + lateral_ampa_name, SynapseMatrixType::SPARSE_INDIVIDUALG, 10,
                            hypercolumn_name, hypercolumn_name_post,
                            update_params_lateral_ampa, update_vars_lateral_ampa,
                            ps_lateral_ampa, {},
                            initConnectivity<InitSparseConnectivitySnippet::FixedProbabilityNoAutapse>(lateralProb));
                        synPop->setMaxDendriticDelayTimesteps(std::rint(maxDelay / model.getDT()));
                    }
                    if (m != mp || n != np) // if between hypercolumns
                    {
                        // NMDA recurrent
                        auto *synPop = model.addSynapsePopulation<BCPNN, PostsynapticModels::ExpCond>(
                            hypercolumn_name + "_to_" + hypercolumn_name_post + lateral_nmda_name, SynapseMatrixType::SPARSE_INDIVIDUALG, 10,
                            hypercolumn_name, hypercolumn_name_post,
                            update_params_lateral_nmda, update_vars_lateral_nmda,
                            ps_lateral_nmda, {},
                            initConnectivity<InitSparseConnectivitySnippet::FixedProbability>(lateralProb));
                        synPop->setMaxDendriticDelayTimesteps(std::rint(maxDelay / model.getDT()));
                    }
                    else
                    {
                        // NMDA recurrent (no autapse)
                        auto *synPop = model.addSynapsePopulation<BCPNN, PostsynapticModels::ExpCond>(
                            hypercolumn_name + "_to_" + hypercolumn_name_post + lateral_nmda_name, SynapseMatrixType::SPARSE_INDIVIDUALG, 10,
                            hypercolumn_name, hypercolumn_name_post,
                            update_params_lateral_nmda, update_vars_lateral_nmda,
                            ps_lateral_nmda, {},
                            initConnectivity<InitSparseConnectivitySnippet::FixedProbabilityNoAutapse>(lateralProb));
                        synPop->setMaxDendriticDelayTimesteps(std::rint(maxDelay / model.getDT()));
                    }
                }
            }

            // add input synapses

            // input connection
            model.addSynapsePopulation<WeightUpdateModels::StaticPulse, PostsynapticModels::ExpCond>(
                hypercolumn_name + input_basename + "_synapse", SynapseMatrixType::SPARSE_GLOBALG, NO_DELAY,
                hypercolumn_name + input_basename, hypercolumn_name,
                {}, s_input_ampa,
                ps_input_ampa, {},
                initConnectivity<InitSparseConnectivitySnippet::OneToOne>());
        }
    }
}