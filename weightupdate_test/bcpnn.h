#pragma once

// GeNN includes
#include "modelSpec.h"

//----------------------------------------------------------------------------
// BCPNN
//----------------------------------------------------------------------------
class BCPNN : public WeightUpdateModels::Base
{
public:
    DECLARE_WEIGHT_UPDATE_MODEL(BCPNN, 9, 7, 0, 0);

    SET_PARAM_NAMES({
      "tauZi",                  // 0 - Time constant of presynaptic primary trace (ms)
      "tauZj",                  // 1 - Time constant of postsynaptic primary trace (ms)
      "tauP",                   // 2 - Time constant of probability trace
      "fMax",                   // 3 - Maximum firing frequency (Hz)
      "wGain",                   // 4 - Maximum weight
      "deltaT",                 // 5 - spike duration (ms)
      "epsilon",                // 6 - epsilon
      "weightEnabled",          // 7 - Should weights get applied to synapses
      "plasticityEnabled",});    // 8 - Should weights be updated

    SET_VARS({{"g", "scalar"}, {"Pij", "scalar"}, {"lastUpdateTime", "scalar"},
              {"Zi", "scalar"}, {"Pi", "scalar"},{"Zj", "scalar"}, {"Pj", "scalar"}});

    SET_DERIVED_PARAMS({
        {"Ai", [](const std::vector<double> &pars, double){ return 1000.0 / (pars[3] * (pars[0] - pars[2])); }},   // 1000 / (fMax * (tauZi-tauP))
        {"Aj", [](const std::vector<double> &pars, double){ return 1000.0 / (pars[3] * (pars[1] - pars[2])); }},   // 1000 / (fMax * (tauZj-tauP))
        {"Aij", [](const std::vector<double> &pars, double){ return (1000000.0 / (pars[0] + pars[1])) / ((pars[3]  * pars[3]) * ((1.0 / ((1.0 / pars[0]) + (1.0 / pars[1]))) - pars[2])); }}, // 1000*1000/(tuaZi+tauZj...)
        {"spikePre", [](const std::vector<double> &pars, double){ return 1000.0 / (pars[3] * pars[5] * pars[0]); }},
        {"spikePost", [](const std::vector<double> &pars, double){ return 1000.0 / (pars[3] * pars[5] * pars[1]); }},
        {"tauZi_1", [](const std::vector<double> &pars, double){ return 1.0/ pars[0]; }}, 
        {"tauZj_1", [](const std::vector<double> &pars, double){ return 1.0/ pars[1]; }},
        {"tauP_1", [](const std::vector<double> &pars, double){ return 1.0/ pars[2]; }},
        {"epsilon_pre", [](const std::vector<double> &pars, double){ return pars[6]/ pars[0]; }}, 
        {"epsilon_post", [](const std::vector<double> &pars, double){ return pars[6]/ pars[1]; }}});  // epsilon= 1000/(fmax*tauP)

    // SET_PRE_SPIKE_CODE(
    //     "$(Zi) = $(Zi) + DT*$(spikePre);\n"
    //     "\n");

    //     // "const scalar dt = $(t) - $(sT_pre);\n"
    //     // "$(ZiStar) = ($(ZiStar) * exp(-dt / $(tauZi))) + 1.0;\n"
    //     // "$(PiStar) = ($(PiStar) * exp(-dt / $(tauP))) + 1.0;\n");

    // SET_POST_SPIKE_CODE(
    //     "$(Zj) = $(Zj) + DT*$(spikePost);\n"
    //     "\n");

    //     // "const scalar dt = $(t) - $(sT_post);\n"
    //     // "$(ZjStar) = ($(ZjStar) * exp(-dt / $(tauZj))) + 1.0;\n"
    //     // "$(PjStar) = ($(PjStar) * exp(-dt / $(tauP))) + 1.0;\n");

    SET_SIM_CODE(
        "if($(weightEnabled)) {\n"
        "   $(addToInSyn, $(g));\n"
        "}\n"
        "if($(plasticityEnabled)){"
        "   $(Zi) = $(Zi) + DT*$(spikePre);\n"
        "}\n"

        // "   const scalar timeSinceLastUpdate = $(t) - $(lastUpdateTime);\n"
        // "   const scalar timeSinceLastPost = $(t) - $(sT_post);\n"
        // "   const scalar newZjStar = $(ZjStar) * exp(-timeSinceLastPost / $(tauZj));\n"
        // "   const scalar newPjStar = $(PjStar) * exp(-timeSinceLastPost / $(tauP));\n"
        // "   $(PijStar) = ($(PijStar) * exp(-timeSinceLastUpdate / $(tauP))) + newZjStar;\n"
        // "   const scalar Pi = $(Ai) * ($(ZiStar) - $(PiStar));\n"
        // "   const scalar Pj = $(Aj) * (newZjStar - newPjStar);\n"
        // "   const scalar Pij = $(Aij) * (($(ZiStar) * newZjStar) - $(PijStar));\n"
        // "   const scalar logPij = log(Pij + ($(Epsilon) * $(Epsilon)));\n"
        // "   const scalar logPiPj = log((Pi + $(Epsilon)) * (Pj + $(Epsilon)));\n"
        // "   $(g) = logPij - logPiPj;\n"
        // "   $(lastUpdateTime) = $(t);\n"

        "\n");

    SET_LEARN_POST_CODE(
        "if($(plasticityEnabled)) {\n"
        "   $(Zj) = $(Zj) + DT*$(spikePost);\n"

        // "   const scalar timeSinceLastUpdate = $(t) - $(lastUpdateTime);\n"
        // "   const scalar timeSinceLastPre = $(t) - $(sT_pre);\n"
        // "   const scalar newZiStar = $(ZiStar) * exp(-timeSinceLastPre / $(tauZi));\n"
        // "   const scalar newPiStar = $(PiStar) * exp(-timeSinceLastPre / $(tauP));\n"
        // "   $(PijStar) = ($(PijStar) * exp(-timeSinceLastUpdate / $(tauP))) + newZiStar;\n"
        // "   const scalar Pi = $(Ai) * (newZiStar - newPiStar);\n"
        // "   const scalar Pj = $(Aj) * ($(ZjStar) - $(PjStar));\n"
        // "   const scalar Pij = $(Aij) * ((newZiStar * $(ZjStar)) - $(PijStar));\n"
        // "   const scalar logPij = log(Pij + ($(Epsilon) * $(Epsilon)));\n"
        // "   const scalar logPiPj = log((Pi + $(Epsilon)) * (Pj + $(Epsilon)));\n"
        // "   $(g) = logPij - logPiPj;\n"
        // "   $(lastUpdateTime) = $(t);\n"
        "}\n");

    SET_SYNAPSE_DYNAMICS_CODE(
        "if($(plasticityEnabled)) {\n"
        "   $(Zi) = $(Zi) + DT * ($(epsilon_pre) - $(Zi)*$(tauZi_1));"
        "   $(Zj) = $(Zj) + DT * ($(epsilon_post)- $(Zj)*$(tauZj_1));"
        "   $(Pi) = $(Pi) + DT * $(tauP_1) * ($(Zi) - $(Pi));"  
        "   $(Pj) = $(Pj) + DT * $(tauP_1) * ($(Zj) - $(Pj));"
        "   $(Pij) = $(Pij) + DT * $(tauP_1) * ($(Zj)*$(Zi) - $(Pij));"
        "   $(g) = $(wGain)*log($(Pij)/($(Pi)*$(Pj)));"
        "}\n"
        "\n");

    SET_NEEDS_PRE_SPIKE_TIME(true);
    SET_NEEDS_POST_SPIKE_TIME(true);
};

IMPLEMENT_MODEL(BCPNN);