
//----------------------------------------------------------------------------
// PostsynapticModels::CondSynapse
//----------------------------------------------------------------------------
//! Synapse based on Tully et al. (2016)
/*! This model has no variables and two parameters:
  - \c tau : Decay time constant
  - \c E : Reversal potential

  \c tau is used by the derived parameter \c expdecay which returns expf(-dt/tau). */
class CondSynapse : public Base
{
public:
    DECLARE_MODEL(CondSynapse, 2, 0);

    SET_DECAY_CODE("$(inSyn)*=$(expDecay);");  // decay should happen in weights/conductance ??

    SET_CURRENT_CONVERTER_CODE("$(inSyn) * ($(E) - $(V))");  //inSyn is treated as a conductance

    SET_PARAM_NAMES({"tau", "E"});

    SET_DERIVED_PARAMS({{"expDecay", [](const std::vector<double> &pars, double dt){ return std::exp(-dt / pars[0]); }}});
};
