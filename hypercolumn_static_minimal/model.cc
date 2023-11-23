// Model definintion file model.cc

#include "modelSpec.h"
#include <neuronModels.h>
#include "SimpleAdEx.h"
#include "AdEx.h"
#include "StaticPulseDendriticDelayStd.h"
#include "model_param.h"

#include <string>

void modelDefinition(ModelSpec &model)
{
    // definition of model
    const std::string model_name = "hypercolumn";
    model.setDT(time_step);           // time step in ms
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
        150.0,   // 6 - adaption time constant [ms]
        0.0    // 7 - adatpion current per spike [nA]  (150 pA)
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
        0.0                          // 7 - adatpion current per spike [nA]
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

    float lateral_ampa_conductance = 4*0.00602; // 6.02 nS
    float lateral_nmda_conductance = 4*0.00122; // 1.22 nS

    // Build distribution for delay parameters
    float maxDelay = 10.0;
    InitVarSnippet::NormalClippedDelay::ParamValues dDist(
        2.0,       // 0 - mean
        0.5,       // 1 - sd
        0.0,       // 2 - min
        maxDelay); // 3 - max

    InitVarSnippet::NormalClippedDelay::ParamValues xDist(
        1.0,  // 0 - mean
        0.0,  // 1 - sd
        0.0,  // 2 - min
        1.0); // 3 - max

    StaticPulseDendriticDelayStd::VarValues update_vars_lateral_ampa(
        lateral_ampa_conductance,                           // 0 - conductance
        initVar<InitVarSnippet::NormalClippedDelay>(dDist), // 1 - delay in ms?
        initVar<InitVarSnippet::NormalClipped>(xDist));     // 2 - STD depletion variable

    StaticPulseDendriticDelayStd::ParamValues update_params_lateral_ampa(
        800.0, // 0 tau recovery time [ms]
        0.25); // 1 spike depletion fraction

    PostsynapticModels::ExpCond::ParamValues ps_lateral_ampa(
        5.0,  // 0 - tau_S: decay time constant for S [ms]
        0.0); // 1 - Erev: Reversal potential AMPA

    StaticPulseDendriticDelayStd::VarValues update_vars_lateral_nmda(
        lateral_nmda_conductance,                           // 0 - conductance
        initVar<InitVarSnippet::NormalClippedDelay>(dDist), // 1 - delay in ms?
        initVar<InitVarSnippet::NormalClipped>(xDist));     // 2 - STD depletion variable

    StaticPulseDendriticDelayStd::ParamValues update_params_lateral_nmda(
        800.0, // 0 tau recovery time [ms]
        0.25); // 1 spike depletion fraction

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
        0.005); // 0 - conductance 5 nS
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
            hypercolumn_name = hypercolumn_basename + std::to_string(m) + "_" + std::to_string(n) + "_";

            // add minicolumns
            for (int i = 0; i < N_minicolumns; i++)
            {
                auto *pop = model.addNeuronPopulation<SimpleAdEx>(hypercolumn_name + minicolumn_basename + std::to_string(i), N_pyramidal, p_pyramidal, ini_pyramidal);
                pop->setSpikeRecordingEnabled(true);
            }

            // add basket pop
            auto *pop = model.addNeuronPopulation<SimpleAdEx>(hypercolumn_name + baskets_name, N_basket, p_basket, ini_basket);
            pop->setSpikeRecordingEnabled(true);

            // add input neurons
            for (int i = 0; i < N_minicolumns; i++)
            {
                if (true) // only input to minicolumns 1 in hypercolumn_0_0  // todo find way to initialize in loop or to not have to initialize
                {
                    auto *pop = model.addNeuronPopulation<NeuronModels::Poisson>(hypercolumn_name + minicolumn_basename + std::to_string(i) + "_" + input_basename, N_pyramidal, p_stim, stim_ini);
                    pop->setSpikeRecordingEnabled(true);
                }
            }
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

            // AMPA (+positive) lateral connections between hypercolumn (and within)
            for (int i = 0; i < N_minicolumns; i++) // presynaptic index j
            {
                for (int mp = 0; mp < hyper_height; mp++) // mp = postsynaptic hypercolum row number
                {
                    for (int np = 0; np < hyper_width; np++) // np = postsynaptic hypercolum column number
                    {
                        hypercolumn_name_post = hypercolumn_basename + std::to_string(mp) + "_" + std::to_string(np) + "_";

                        for (int j = 0; j < N_minicolumns; j++) // postsynaptic index i
                        {
                            if (i == j)
                            {
                                if (m != mp && n != np) // if between hypercolumns
                                {
                                    // AMPA recurrent
                                    auto *synPop = model.addSynapsePopulation<StaticPulseDendriticDelayStd, PostsynapticModels::ExpCond>(
                                        hypercolumn_name + "to_" + hypercolumn_name_post + minicolumn_basename + std::to_string(i) + "to_" + std::to_string(j) + "_" + lateral_ampa_name, SynapseMatrixType::SPARSE_INDIVIDUALG, 10,
                                        hypercolumn_name + minicolumn_basename + std::to_string(i), hypercolumn_name_post + minicolumn_basename + std::to_string(j),
                                        update_params_lateral_ampa, update_vars_lateral_ampa,
                                        ps_lateral_ampa, {},
                                        initConnectivity<InitSparseConnectivitySnippet::FixedProbability>(lateralProb));

                                    synPop->setMaxDendriticDelayTimesteps(std::rint(maxDelay / model.getDT()));
                                }
                                else
                                {
                                    // AMPA recurrent (no autapse)
                                    auto *synPop = model.addSynapsePopulation<StaticPulseDendriticDelayStd, PostsynapticModels::ExpCond>(
                                        hypercolumn_name + "to_" + hypercolumn_name_post + minicolumn_basename + std::to_string(i) + "to_" + std::to_string(j) + "_" + lateral_ampa_name, SynapseMatrixType::SPARSE_INDIVIDUALG, 10,
                                        hypercolumn_name + minicolumn_basename + std::to_string(i), hypercolumn_name_post + minicolumn_basename + std::to_string(j),
                                        update_params_lateral_ampa, update_vars_lateral_ampa,
                                        ps_lateral_ampa, {},
                                        initConnectivity<InitSparseConnectivitySnippet::FixedProbabilityNoAutapse>(lateralProb));
                                    synPop->setMaxDendriticDelayTimesteps(std::rint(maxDelay / model.getDT()));
                                }
                            }
                            else
                            {
                                // AMPA non-recurrent (not connected in static model)
                                // auto *synPop = model.addSynapsePopulation<StaticPulseDendriticDelayStd, PostsynapticModels::ExpCond>(
                                //     hypercolumn_name + minicolumn_basename + std::to_string(i) + "to" + std::to_string(j) + "_" + lateral_ampa_name, SynapseMatrixType::SPARSE_INDIVIDUALG, 10,
                                //      hypercolumn_name + minicolumn_basename + std::to_string(j), hypercolumn_name + minicolumn_basename + std::to_string(i),
                                //     update_params_lateral_ampa, update_vars_lateral_ampa,
                                //     ps_lateral_ampa, {},
                                //     initConnectivity<InitSparseConnectivitySnippet::FixedProbability>(lateralProb));
                                // synPop->setMaxDendriticDelayTimesteps(std::rint(maxDelay / model.getDT()));
                            }
                        }
                    }
                }
            }

            // NMDA (+positive) lateral connections between hypercolumn (and within)
            for (int i = 0; i < N_minicolumns; i++) // presynaptic index i
            {
                for (int mp = 0; mp < hyper_height; mp++) // mp = postsynaptic hypercolum row number
                {
                    for (int np = 0; np < hyper_width; np++) // np = postsynaptic hypercolum column number
                    {
                        hypercolumn_name_post = hypercolumn_basename + std::to_string(mp) + "_" + std::to_string(np) + "_";

                        for (int j = 0; j < N_minicolumns; j++) // postsynaptic index j
                        {
                            if ((i + 1) % N_minicolumns == j) // ring connectivity / next minicolumn  // somehow leads to feedback
                            {
                                if (m != mp && n != np) // if between hypercolumns
                                {
                                    // NMDA recurrent
                                    auto *synPop = model.addSynapsePopulation<StaticPulseDendriticDelayStd, PostsynapticModels::ExpCond>(
                                        hypercolumn_name + "to_" + hypercolumn_name_post + minicolumn_basename + std::to_string(i) + "to_" + std::to_string(j) + "_" + lateral_nmda_name, SynapseMatrixType::SPARSE_INDIVIDUALG, 10,
                                        hypercolumn_name + minicolumn_basename + std::to_string(i), hypercolumn_name_post + minicolumn_basename + std::to_string(j),
                                        update_params_lateral_nmda, update_vars_lateral_nmda,
                                        ps_lateral_nmda, {},
                                        initConnectivity<InitSparseConnectivitySnippet::FixedProbability>(lateralProb));
                                    synPop->setMaxDendriticDelayTimesteps(std::rint(maxDelay / model.getDT()));
                                }
                                else
                                {
                                    // NMDA recurrent (no autapse)
                                    auto *synPop = model.addSynapsePopulation<StaticPulseDendriticDelayStd, PostsynapticModels::ExpCond>(
                                        hypercolumn_name + "to_" + hypercolumn_name_post + minicolumn_basename + std::to_string(i) + "to_" + std::to_string(j) + "_" + lateral_nmda_name, SynapseMatrixType::SPARSE_INDIVIDUALG, 10,
                                        hypercolumn_name + minicolumn_basename + std::to_string(i), hypercolumn_name_post + minicolumn_basename + std::to_string(j),
                                        update_params_lateral_nmda, update_vars_lateral_nmda,
                                        ps_lateral_nmda, {},
                                        initConnectivity<InitSparseConnectivitySnippet::FixedProbabilityNoAutapse>(lateralProb));
                                    synPop->setMaxDendriticDelayTimesteps(std::rint(maxDelay / model.getDT()));
                                }
                            }
                            else
                            {
                                // NMDA non-recurrent (not connected in static model)
                                // model.addSynapsePopulation<StaticPulseDendriticDelayStd, PostsynapticModels::ExpCond>(
                                //     hypercolumn_name + minicolumn_basename + std::to_string(i) + "to" + std::to_string(j) + "_" + lateral_nmda_name, SynapseMatrixType::SPARSE_INDIVIDUALG, 10,
                                //      hypercolumn_name + minicolumn_basename + std::to_string(j), hypercolumn_name + minicolumn_basename + std::to_string(i),
                                //     update_params_lateral_nmda, update_vars_lateral_nmda,
                                //     ps_lateral_nmda, {},
                                //     initConnectivity<InitSparseConnectivitySnippet::FixedProbability>(lateralProb));
                            }
                        }
                    }
                }
            }

            // add input synapses

            for (int i = 0; i < N_minicolumns; i++)
            {
                // input connection
                if (true) // only input to minicolumns 1 in hypercolumn_0_0
                {
                    // todo fix names
                    model.addSynapsePopulation<WeightUpdateModels::StaticPulse, PostsynapticModels::ExpCond>(
                        hypercolumn_name + minicolumn_basename + std::to_string(i) + "_" + input_basename + "_synapse", SynapseMatrixType::SPARSE_GLOBALG, NO_DELAY,
                        hypercolumn_name + minicolumn_basename + std::to_string(i) + "_" + input_basename, hypercolumn_name + minicolumn_basename + std::to_string(i),
                        {}, s_input_ampa,
                        ps_input_ampa, {},
                        initConnectivity<InitSparseConnectivitySnippet::OneToOne>());
                }
            }
        }
    }

    // todo input neurons, better way to test: array of pointer generator
    // todo background poisson generators, what is meant here; ask anders
    // todo distribution for strength and delay, values; ask anders
    // todo noisy neurons? ask anders
    // todo (euclidian) delay
}