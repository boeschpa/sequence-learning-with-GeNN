#include "neuronModels.h"

//----------------------------------------------------------------------------
// NeuronModels::AdEx
//----------------------------------------------------------------------------
// AdEx IF-Neuron


class CAdEx : public NeuronModels::Base
{
public:
    DECLARE_MODEL(CAdEx, 18, 7);

    SET_SIM_CODE(
        //"unsigned int mt;\n"
        //"scalar mdt= DT/10.0;\n"

        //"for (mt=0; mt < 10; mt++) {\n"
        "$(Ga) = $(Ga) + DT * $(TauA1) * ($(GaMax) / (1 + exp(($(Va)-$(V))/$(VaSlope))) - $(Ga) ) ;\n"
        //   uS   =  uS   + ms  *    1/ms  * ( uS   * (  1  + exp((  mV -  mV)/    mV    ) -   uS  )
        "$(V) = $(V) + DT * $(C1) * ( $(G) * ( $(Vrest) - $(V) ) + $(Vslope) * $(G) * exp( ( $(V) - $(Vthresh) ) * $(Vslope1) ) - ($(Ea)-$(V))*$(Ga) + $(Isyn) );\n"  // Isyn includes all synaptic currents + intrinsic excitability
        //   mV  =  mV  + ms  * 1/nF  * (  uS  *  ( mV  -  mV      ) +  mV       *  uS  * exp( (  mV  -  mV        ) *  1/mV      ) -  (mV - mV ) * uS   +  nA    )
        "$(Zj) = $(Zj) + DT * ($(epsilonZ)- $(Zj)*$(tauZ_1));\n"
        "$(Pj) = $(Pj) + DT * $(kappa) * $(tauP_1) * ($(Zj) - $(Pj));\n"
        "$(Ib) = $(biasGainBase)*log($(Pj));" 
        "\n"
    );

    SET_THRESHOLD_CONDITION_CODE("$(V) >= $(Vthresh)");

    SET_RESET_CODE(
        "$(V) = $(Vreset);\n"
        "$(Ga) = $(Ga) + $(b) ;\n"
        "$(Zj) = $(Zj) + DT*$(spikePost);\n");

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
        "GaMax",    // 11 max subthreshold adaptation conductance (uS)
        "TauZ",       // 12
        "TauP",       // 13
        "epsilon",    // 14
        "deltaT",     // 15
        "fMax",       // 16
        "biasGainBase"}); // 17

    SET_DERIVED_PARAMS({
        {"TauA1", [](const std::vector<double> &pars, double){ return 1.0 / pars[6]; }},  // TauW^-1
        {"Vslope1", [](const std::vector<double> &pars, double){ return 1.0 / pars[5]; }}, // Vslope^-1
        {"C1", [](const std::vector<double> &pars, double){ return 1.0 / pars[0]; }}, // C^-1
        {"epsilonZ", [](const std::vector<double> &pars, double){ return pars[14]/ pars[12]; }}, // epsilon/tauZ
        {"tauZ_1", [](const std::vector<double> &pars, double){ return 1.0/ pars[12]; }}, // 1.0 / tauZj
        {"tauP_1", [](const std::vector<double> &pars, double){ return 1.0/ pars[15]; }},  // 1.0 / tauP
        {"spikePost", [](const std::vector<double> &pars, double){ return 1000.0 / (pars[16] * pars[15] * pars[12]); }}}); // 1000 / (fMAx * deltaT * tauZj)

    SET_VARS({{"V", "scalar"}, {"Ga", "scalar"}, {"Zj", "scalar"}, {"Pj", "scalar"}, {"Ib", "scalar"}, {"kappa", "scalar"}, {"biasGain", "scalar"}});

    SET_NEEDS_AUTO_REFRACTORY(true);
};

IMPLEMENT_MODEL(CAdEx);

