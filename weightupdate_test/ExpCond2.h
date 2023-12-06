#pragma once

// GeNN includes
#include "modelSpec.h"

//----------------------------------------------------------------------------
// PostsynapticModels::ExpCond
//----------------------------------------------------------------------------
//! Exponential decay with synaptic input treated as a conductance value.
/*! This model has no variables and two parameters:
  - \c tau : Decay time constant
  - \c E : Reversal potential

  \c tau is used by the derived parameter \c expdecay which returns expf(-dt/tau). */
class ExpCond2 : public PostsynapticModels::Base
{
public:
    DECLARE_MODEL(ExpCond2, 2, 0);

    SET_DECAY_CODE(
        "$(inSyn)*=$(expDecay);\n"
        "$(inSyn2)=0.0;\n");

    SET_APPLY_INPUT_CODE(
      "$(Sj) += $(inSyn2);\n"
      "$(Isyn) += $(inSyn) * ($(E) - $(V));\n"
    );

    SET_PARAM_NAMES({"tau", "E"});

    SET_DERIVED_PARAMS({{"expDecay", [](const std::vector<double> &pars, double dt){ return std::exp(-dt / pars[0]); }}});
};

IMPLEMENT_MODEL(ExpCond2);