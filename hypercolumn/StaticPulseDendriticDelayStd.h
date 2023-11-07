#include "weightUpdateModels.h"


class StaticPulseDendriticDelayStd : public WeightUpdateModels::Base
{
public:
    DECLARE_WEIGHT_UPDATE_MODEL(StaticPulseDendriticDelayStd, 2, 3, 0, 0);

    SET_PARAM_NAMES({"Tau", "U"});

    SET_VARS({{"g", "scalar", VarAccess::READ_ONLY}, {"d", "uint8_t", VarAccess::READ_ONLY}, {"x", "scalar", VarAccess::READ_WRITE}});
    
    SET_DERIVED_PARAMS({
        {"Tau1", [](const std::vector<double> &pars, double){ return 1.0 / pars[0]; }}});  // Tau^-1

    // SET_SYNAPSE_DYNAMICS_CODE(
    //     "$(x) = $(x) + DT * $(Tau1) * (1.0 - $(x));\n"
    // )

    // delay leads to invalid % operation with float (d is float?)
    SET_SIM_CODE(
        "$(addToInSyn, $(g));\n"
        "$(x) = $(x) - $(U) * 1.0;\n"  // x is not a variable but a constant 1.0
    );
};

IMPLEMENT_MODEL(StaticPulseDendriticDelayStd);

