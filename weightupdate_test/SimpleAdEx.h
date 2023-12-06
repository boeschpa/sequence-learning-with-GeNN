#include "neuronModels.h"

//----------------------------------------------------------------------------
// NeuronModels::SimpleAdEx
//----------------------------------------------------------------------------
// simplyfied AdEx IF-Neuron based on Tully et al. 2016

class SimpleAdEx : public NeuronModels::Base
{
public:
    DECLARE_MODEL(SimpleAdEx, 14, 7);

    SET_SIM_CODE(
        // todo add micro steps
        //"unsigned int mt;\n"
        //"scalar mdt= DT;\n"

        //"for (mt=0; mt < 1; mt++) {\n"
        //"   $(Iw) = $(Iw) * ( 1.0 - mdt * $(TauW1) )  ;\n"
        "$(Iw) = $(Iw) * $(ExpW);\n"
        //nA   =  nA   * ( 1   - ms  *  1/ms   )
        "$(V) = $(V) + DT * $(C1) * ( $(G) * ( $(Vrest) - $(V) ) + $(Vslope) * $(G) * exp( ( $(V) - $(Vthresh) ) * $(Vslope1) ) - $(Iw) + $(Isyn) + $(biasGain)*$(Ib));\n"  // Isyn includes all synaptic currents + intrinsic excitability
        //mV  =  mV  + ms  * 1/nF  * (  uS  *  ( mV  -  mV      ) +  mV       *  uS  * exp( (  mV  -  mV        ) *  1/mV      ) -  nA   +  nA    )
        "$(Zj) = $(Zj) + DT * ($(epsilonZ)- $(Zj)*$(tauZ_1));\n"
        "$(Pj) = $(Pj) + DT * $(kappa) * $(tauP_1) * ($(Zj) - $(Pj));\n"
        "$(Ib) = $(biasGainBase)*log($(Pj));"
        
        "\n"
    );

    SET_THRESHOLD_CONDITION_CODE("$(V) >= $(Vthresh)");

    SET_RESET_CODE(
        "$(V) = $(Vreset);\n"
        "$(Iw) = $(Iw) + $(b) ;\n"
        "$(Zj) = $(Zj) + DT*$(spikePost);\n");

    SET_PARAM_NAMES({
        "C",          // 0 Membrane capacitance [nF]
        "G",          // 1 Membrane leak conductance [uS]
        "Vrest",      // 2 Resting membrane potential [mV]
        "Vreset",     // 3 Reset voltage [mV]
        "Vthresh",    // 4 Spiking threshold [mV]
        "Vslope",     // 5 spike upstroke slopefactor [mV]
        "TauW",       // 6 adaption time constant [ms]
        "b",          // 7 adaption current spike increase [nA]
        "TauZ",       // 8
        "TauP",       // 9
        "epsilon",    // 10
        "deltaT",     // 11
        "fMax",       // 12
        "biasGainBase"}); // 13
        

    SET_DERIVED_PARAMS({
        {"TauW1", [](const std::vector<double> &pars, double){ return 1.0 / pars[6]; }},  // TauW^-1
        {"Vslope1", [](const std::vector<double> &pars, double){ return 1.0 / pars[5]; }}, // Vslope^-1
        {"C1", [](const std::vector<double> &pars, double){ return 1.0 / pars[0]; }}, // C^-1
        {"epsilonZ", [](const std::vector<double> &pars, double){ return pars[10]/ pars[8]; }}, // epsilon/tauZ
        {"tauZ_1", [](const std::vector<double> &pars, double){ return 1.0/ pars[8]; }}, // 1.0 / tauZj
        {"tauP_1", [](const std::vector<double> &pars, double){ return 1.0/ pars[9]; }},  // 1.0 / tauP
        {"spikePost", [](const std::vector<double> &pars, double){ return 1000.0 / (pars[12] * pars[11] * pars[8]); }}, // 1000 / (fMAx * deltaT * tauZj)
        {"ExpW", [](const std::vector<double> &pars, double dt){ return std::exp(-dt/25.0/pars[6]); }}}); // C^-1

    SET_VARS({{"V", "scalar"}, {"Iw", "scalar"}, {"Zj", "scalar"}, {"Pj", "scalar"}, {"Ib", "scalar"}, {"kappa", "scalar"}, {"biasGain", "scalar"}});

    SET_NEEDS_AUTO_REFRACTORY(false);
};

IMPLEMENT_MODEL(SimpleAdEx);

