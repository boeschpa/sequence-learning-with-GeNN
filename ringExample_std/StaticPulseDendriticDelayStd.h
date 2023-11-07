#include "weightUpdateModels.h"


class StaticPulseDendriticDelayStd : public WeightUpdateModels::Base
{
public:
    DECLARE_WEIGHT_UPDATE_MODEL(StaticPulseDendriticDelayStd, 2, 3, 0, 0);

    SET_PARAM_NAMES({"Tau", "U"});

    SET_VARS({{"g", "scalar"}, {"d", "uint16_t"}, {"x", "scalar"}});
    
    SET_DERIVED_PARAMS({
        {"Tau1", [](const std::vector<double> &pars, double){ return 1.0 / pars[0]; }}});  // Tau^-1

    SET_SYNAPSE_DYNAMICS_CODE(
        "$(x) = $(x) + DT * $(Tau1) * (1.0 - $(x));\n"
    )

    // delay leads to invalid % operation with float (d is float?)
    SET_SIM_CODE(
        "$(addToInSynDelay, $(x)*$(g), $(d));\n"
        "$(x)=($(x) - $(U)*$(x));\n"  // x is not a variable but a constant 1.0
    );
};

IMPLEMENT_MODEL(StaticPulseDendriticDelayStd);

