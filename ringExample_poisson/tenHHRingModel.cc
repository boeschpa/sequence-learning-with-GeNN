// Model definintion file tenHHRing.cc
#include "modelSpec.h"
#include "MatrixConnectivity.h"
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
    // definition of tenHHRing
    model.setDT(0.1);
    model.setName("tenHHRing");
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

    NeuronModels::Poisson::ParamValues p_stim(
        1.0,  // Refractory period (ms)
        1.0,    //  duration of spike (ms)
        0.0,    // Membrane potential at spike (mV)
        -60.0); // Membrane potential at rest (mV)

    NeuronModels::Poisson::VarValues stim_ini(
        uninitialisedVar(),     // V - Membrane potential
        uninitialisedVar());    // SpikeTime - Time at which the neuron spiked for the last time


    model.addNeuronPopulation<NeuronModels::TraubMiles>("Pop1", 10, p, ini);
    model.addNeuronPopulation<NeuronModels::Poisson>("Stim", 2, p_stim, stim_ini);
    WeightUpdateModels::StaticPulse::VarValues s_ini(
         -0.2); // 0 - g: the synaptic conductance value
    PostsynapticModels::ExpCond::ParamValues ps_p(
        1.0,    // 0 - tau_S: decay time constant for S [ms]
        -80.0); // 1 - Erev: Reversal potential
    
    // // create ring matrix
    // float matrix[10][10] = {{0.0}};
    // for (int m = 0; m<10; m++){
    //     for (int n = 0; n<10; n++){
    //         if (n == (m + 1) % 10){
    //             matrix[m][n] = 1.0;
    //         }
    //     }
    // }
    // MatrixConnectivity::ParamValues matrixParam(matrix);
    Ring::ParamValues ringParam(1);

    // model.addSynapsePopulation<WeightUpdateModels::StaticPulse, PostsynapticModels::ExpCond>(
    //     "Pop1self", SynapseMatrixType::SPARSE_GLOBALG, 100,
    //     "Pop1", "Pop1",
    //     {}, s_ini,
    //     ps_p, {},
    //     initConnectivity<Ring>(ringParam));
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