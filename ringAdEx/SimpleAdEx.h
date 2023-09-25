#include "neuronModels.h"

//----------------------------------------------------------------------------
// NeuronModels::SimpleAdEx
//----------------------------------------------------------------------------
// simplyfied AdEx IF-Neuron based on Tully et al. 2016


//         "scalar Imem;\n"
//         "unsigned int mt;\n"
//         "scalar mdt= DT/25.0;\n"
//         "for (mt=0; mt < 25; mt++) {\n"

class SimpleAdEx : public NeuronModels::Base
{
public:
    DECLARE_MODEL(SimpleAdEx, 8, 2);

    SET_SIM_CODE(
        // todo add micro steps
        "$(Iw) = $(Iw) * ( 1.0 - DT * 1.0e3 * $(TauW1) )  ;\n"
        //pA   =  pA   * ( 1   - ms          *  1/ms   )
        "$(V) = $(V) + DT * 1.0e3 * $(C1) * ( $(G) * ( $(Vrest) - $(V) ) + $(Vslope) * $(G) * exp( ( $(V) - $(Vthresh) ) * $(Vslope1) ) - $(Iw) + $(Isyn) );\n"  // Isyn includes all synaptic currents + intrinsic excitability
        //mV  =  mV  + ms         * 1/pF  * (  uS  *  ( mV  -  mV      ) +  mV       *  uS  * exp( (  mV  -  mV        ) *  1/mV      ) -  pA   +  pA ?    )
    );

    SET_THRESHOLD_CONDITION_CODE("$(V) >= $(Vthresh)");

    SET_RESET_CODE(
        "$(V) = $(Vreset);\n"
        "$(Iw) = $(Iw) + $(b) ;\n");

    SET_PARAM_NAMES({
        "C",          // Membrane capacitance [pF]
        "G",          // Membrane leak conductance [uS]
        "Vrest",      // Resting membrane potential [mV]
        "Vreset",     // Reset voltage [mV]
        "Vthresh",    // Spiking threshold [mV]
        "Vslope",     // spike upstroke slopefactor [mV]
        "TauW",       // adaption time constant [ms]
        "b"});          // adaption current spike increase [pA]
        

    SET_DERIVED_PARAMS({
        {"TauW1", [](const std::vector<double> &pars, double){ return 1.0 / pars[6]; }},  // TauW^-1
        {"Vslope1", [](const std::vector<double> &pars, double){ return 1.0 / pars[5]; }}, // Vslope^-1
        {"C1", [](const std::vector<double> &pars, double){ return 1.0 / pars[0]; }}}); // C^-1

    SET_VARS({{"V", "scalar"}, {"Iw", "scalar"}});

    SET_NEEDS_AUTO_REFRACTORY(false);
};

IMPLEMENT_MODEL(SimpleAdEx);

