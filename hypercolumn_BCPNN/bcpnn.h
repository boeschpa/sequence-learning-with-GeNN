#pragma once

// GeNN includes
#include "modelSpec.h"

//----------------------------------------------------------------------------
// BCPNN
//----------------------------------------------------------------------------
class BCPNN : public WeightUpdateModels::Base
{
public:
    DECLARE_WEIGHT_UPDATE_MODEL(BCPNN, 8, 11, 0, 0);

    SET_PARAM_NAMES({
      "tauZi",                  // 0 - Time constant of presynaptic primary trace (ms)
      "tauZj",                  // 1 - Time constant of postsynaptic primary trace (ms)
      "tauP",                   // 2 - Time constant of probability trace
      "fMax",                   // 3 - Maximum firing frequency (Hz)
      "deltaT",                 // 4 - spike duration (ms)
      "epsilon",                // 5 - epsilon
      "tauX",                   // 6 - short term depression time constant
      "U"});                     // 7 - depletion fraction

    SET_VARS({{"g", "scalar"}, {"Pij", "scalar"},
              {"Zi", "scalar"}, {"Pi", "scalar"},{"Zj", "scalar"}, {"Pj", "scalar"},{"wGainBase", "scalar"}, 
              {"wGain", "scalar"},{"kappa", "scalar"},{"d", "uint16_t"}, {"x", "scalar"}});

    SET_DERIVED_PARAMS({
        {"Ai", [](const std::vector<double> &pars, double){ return 1000.0 / (pars[3] * (pars[0] - pars[2])); }},   // 1000 / (fMax * (tauZi-tauP))
        {"Aj", [](const std::vector<double> &pars, double){ return 1000.0 / (pars[3] * (pars[1] - pars[2])); }},   // 1000 / (fMax * (tauZj-tauP))
        {"Aij", [](const std::vector<double> &pars, double){ return (1000000.0 / (pars[0] + pars[1])) / ((pars[3]  * pars[3]) * ((1.0 / ((1.0 / pars[0]) + (1.0 / pars[1]))) - pars[2])); }}, // 1000*1000/(tuaZi+tauZj...)
        {"spikePre", [](const std::vector<double> &pars, double){ return 1000.0 / (pars[3] * pars[4] * pars[0]); }},
        {"spikePost", [](const std::vector<double> &pars, double){ return 1000.0 / (pars[3] * pars[4] * pars[1]); }},
        {"tauZi_1", [](const std::vector<double> &pars, double){ return 1.0/ pars[0]; }}, 
        {"tauZj_1", [](const std::vector<double> &pars, double){ return 1.0/ pars[1]; }},
        {"tauP_1", [](const std::vector<double> &pars, double){ return 1.0/ pars[2]; }},
        {"TauX1", [](const std::vector<double> &pars, double){ return 1.0 / pars[6]; }},  // TauX^-1
        {"epsilon_pre", [](const std::vector<double> &pars, double){ return pars[5]/ pars[0]; }}, 
        {"epsilon_post", [](const std::vector<double> &pars, double){ return pars[5]/ pars[1]; }}});  // epsilon= 1000/(fmax*tauP)

    SET_SIM_CODE(
        "$(addToInSynDelay, $(wGain)*$(x)*$(g), $(d));\n"
        "$(x)=($(x) - $(U)*$(x));\n"
        "$(Zi) = $(Zi) + DT*$(spikePre);\n"
        "\n");

    SET_LEARN_POST_CODE(
        "$(Zj) = $(Zj) + DT*$(spikePost);\n"
        "\n");

    SET_SYNAPSE_DYNAMICS_CODE(
        "$(x) = $(x) + DT * $(TauX1) * (1.0 - $(x));\n"
        "$(Zi) = $(Zi) + DT * ($(epsilon_pre) - $(Zi)*$(tauZi_1));"
        "$(Zj) = $(Zj) + DT * ($(epsilon_post)- $(Zj)*$(tauZj_1));"
        "$(Pi) = $(Pi) + DT * $(kappa) * $(tauP_1) * ($(Zi) - $(Pi));"  
        "$(Pj) = $(Pj) + DT * $(kappa) * $(tauP_1) * ($(Zj) - $(Pj));"
        "$(Pij) = $(Pij) + DT * $(kappa) * $(tauP_1) * ($(Zj)*$(Zi) - $(Pij));"
        "$(g) = $(wGainBase)*log($(Pij)/($(Pi)*$(Pj)));"
        "\n");

    SET_NEEDS_PRE_SPIKE_TIME(true);
    SET_NEEDS_POST_SPIKE_TIME(true);
};

IMPLEMENT_MODEL(BCPNN);