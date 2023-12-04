#include "neuronModels.h"

//----------------------------------------------------------------------------
// NeuronModels::AdEx
//----------------------------------------------------------------------------
// AdEx IF-Neuron


class CAdEx : public NeuronModels::Base
{
public:
    DECLARE_MODEL(CAdEx, 12, 2);

    SET_SIM_CODE(
        "unsigned int mt;\n"
        "scalar mdt= DT/10.0;\n"

        "for (mt=0; mt < 10; mt++) {\n"
        "   $(Ga) = $(Ga) + mdt * $(TauA1) * ($(GaMax) / (1 + exp(($(Va)-$(V))/$(VaSlope)) - $(Ga) )) ;\n"
        //   uS   =  uS   + ms  *    1/ms  * ( uS   * (  1  + exp((  mV -  mV)/    mV    ) -   uS  )
        "   $(V) = $(V) + mdt * $(C1) * ( $(G) * ( $(Vrest) - $(V) ) + $(Vslope) * $(G) * exp( ( $(V) - $(Vthresh) ) * $(Vslope1) ) - ($(Ea)-$(V))*$(Ga) + $(Isyn) );\n"  // Isyn includes all synaptic currents + intrinsic excitability
        //   mV  =  mV  + ms  * 1/nF  * (  uS  *  ( mV  -  mV      ) +  mV       *  uS  * exp( (  mV  -  mV        ) *  1/mV      ) -  (mV - mV ) * uS   +  nA    )
        "}\n"
    );

    SET_THRESHOLD_CONDITION_CODE("$(V) >= $(Vthresh)");

    SET_RESET_CODE(
        "$(V) = $(Vreset);\n"
        "$(Ga) = $(Ga) + $(b) ;\n");

    SET_PARAM_NAMES({
        "C",          // 0 Membrane capacitance [nF]
        "G",          // 1 Membrane leak conductance [uS]
        "Vrest",      // 2 Resting membrane potential [mV]
        "Vreset",     // 3 Reset voltage [mV]
        "Vthresh",    // 4 Spiking threshold [mV]
        "Vslope",     // 5 spike upstroke slopefactor [mV]
        "TauA",       // 6 adaptation time constant [ms]
        "b",          // 7 adaptation conductance per spike [uS]
        "Ea",         // 8 adaptation reversal potential (mV)
        "Va",         // 9 subthreshold adaptation activation voltage (mV)
        "VaSlope",    // 10 slope of subthreshold adaptation (mV)
        "GaMax"});    // 11 max subthreshold adaptation conductance (uS)

    SET_DERIVED_PARAMS({
        {"TauA1", [](const std::vector<double> &pars, double){ return 1.0 / pars[6]; }},  // TauW^-1
        {"Vslope1", [](const std::vector<double> &pars, double){ return 1.0 / pars[5]; }}, // Vslope^-1
        {"C1", [](const std::vector<double> &pars, double){ return 1.0 / pars[0]; }}}); // C^-1

    SET_VARS({{"V", "scalar"}, {"Ga", "scalar"}});

    SET_NEEDS_AUTO_REFRACTORY(true);
};

IMPLEMENT_MODEL(CAdEx);

