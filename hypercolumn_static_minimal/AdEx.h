#include "neuronModels.h"

//----------------------------------------------------------------------------
// NeuronModels::AdEx
//----------------------------------------------------------------------------
// AdEx IF-Neuron


class AdEx : public NeuronModels::Base
{
public:
    DECLARE_MODEL(AdEx, 9, 2);

    SET_SIM_CODE(
        "unsigned int mt;\n"
        "scalar mdt= DT/10.0;\n"

        "for (mt=0; mt < 10; mt++) {\n"
        "   $(Iw) = $(Iw) + mdt * $(TauW1) * ( $(a) * ( $(V) - $(Vrest) ) - $(Iw)) ;\n"
        //   nA   =  nA   + ms  *    1/ms  * ( uS   * (  mV -    mV     ) -   nA  )
        "   $(V) = $(V) + mdt * $(C1) * ( $(G) * ( $(Vrest) - $(V) ) + $(Vslope) * $(G) * exp( ( $(V) - $(Vthresh) ) * $(Vslope1) ) - $(Iw) + $(Isyn) );\n"  // Isyn includes all synaptic currents + intrinsic excitability
        //   mV  =  mV  + ms  * 1/nF  * (  uS  *  ( mV  -  mV      ) +  mV       *  uS  * exp( (  mV  -  mV        ) *  1/mV      ) -  nA   +  nA    )
        "}\n"
    );

    SET_THRESHOLD_CONDITION_CODE("$(V) >= $(Vthresh)");

    SET_RESET_CODE(
        "$(V) = $(Vreset);\n"
        "$(Iw) = $(Iw) + $(b) ;\n");

    SET_PARAM_NAMES({
        "C",          // Membrane capacitance [nF]
        "G",          // Membrane leak conductance [uS]
        "Vrest",      // Resting membrane potential [mV]
        "Vreset",     // Reset voltage [mV]
        "Vthresh",    // Spiking threshold [mV]
        "Vslope",     // spike upstroke slopefactor [mV]
        "TauW",       // adaption time constant [ms]
        "b",       // adaption current per spike [nA]
        "a"});          // adaption coupling parameter a
        

    SET_DERIVED_PARAMS({
        {"TauW1", [](const std::vector<double> &pars, double){ return 1.0 / pars[6]; }},  // TauW^-1
        {"Vslope1", [](const std::vector<double> &pars, double){ return 1.0 / pars[5]; }}, // Vslope^-1
        {"C1", [](const std::vector<double> &pars, double){ return 1.0 / pars[0]; }}}); // C^-1

    SET_VARS({{"V", "scalar"}, {"Iw", "scalar"}});

    SET_NEEDS_AUTO_REFRACTORY(true);
};

IMPLEMENT_MODEL(AdEx);

