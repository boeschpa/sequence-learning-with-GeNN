import matplotlib.pyplot as plt
import numpy as np
from pygenn.genn_model import (GeNNModel, init_connectivity, create_cmlf_class,
                               create_custom_sparse_connect_init_snippet_class,
                               create_custom_weight_update_class)
from pygenn.genn_wrapper.Models import (VarAccessMode_READ_WRITE, 
                                        VarAccessMode_READ_ONLY,
                                        VarAccess_READ_WRITE, 
                                        VarAccess_READ_ONLY)
from pygenn.genn_wrapper import NO_DELAY
from pygenn import genn_model

#RING SNIPPET
ring_model = create_custom_sparse_connect_init_snippet_class(
    "ring",
    row_build_code=
        """
        $(addSynapse, ($(id_pre) + 1) % $(num_post));
        $(endRow);
        """,
    calc_max_row_len_func=create_cmlf_class(
        lambda num_pre, num_post, pars: 1)())

#MODEL

#NEURON POPULATIONS
model = GeNNModel("float", "tenHHRing")
model.dT = 0.1
p = {"gNa": 7.15,   # Na conductance in [muS]
     "ENa": 50.0,   # Na equi potential [mV]
     "gK": 1.43,    # K conductance in [muS]
     "EK": -95.0,   # K equi potential [mV] 
     "gl": 0.02672, # leak conductance [muS]
     "El": -63.563, # El: leak equi potential in mV, 
     "C": 0.143}    # membr. capacity density in nF
ini = {"V": -60.0,      # membrane potential
       "m": 0.0529324,  # prob. for Na channel activation
       "h": 0.3176767,  # prob. for not Na channel blocking
       "n": 0.5961207}  # prob. for K channel activation
stim_ini = {"startSpike": [0], "endSpike": [1]}
pop1 = model.add_neuron_population("Pop1", 10, "TraubMiles", p, ini)
stim = model.add_neuron_population("Stim", 1, "SpikeSourceArray", {}, stim_ini)

# DEFINE CUSTOM STD UPDATE MODEL
sim_code_std = ("$(addToInSyn, $(g));\n" +
                "$(x)= 1.0;\n")
synapse_dynamics_code_std = ""

std_model =  create_custom_weight_update_class(
    "StaticPulseDendriticDelayStd",
    var_name_types=[("g", "scalar", VarAccess_READ_ONLY),("d","int", VarAccess_READ_WRITE),("x","scalar",VarAccess_READ_WRITE)],
    param_names=["Tau","U"],
    derived_params=[("Tau1", genn_model.create_dpf_class(lambda pars, dt: 1.0 / pars[0])())],
    sim_code = sim_code_std,
    synapse_dynamics_code= synapse_dynamics_code_std)



#SYNAPSES
update_params = {"Tau": 1.0,
                 "U": -80.0}
update_vars = {"g": -0.2,
               "d": 2,
               "x": 1.0}
ps_p = {"tau": 1.0, # Decay time constant [ms] 
        "E": -80.0} # Reversal potential [mV]
# model.add_synapse_population("Pop1self", "SPARSE_GLOBALG", 10,
#     pop1, pop1,
#     "StaticPulse", {}, s_ini, {}, {},
#     "ExpCond", ps_p, {},
#     init_connectivity(ring_model, {}))
model.add_synapse_population("StimPop1", "SPARSE_GLOBALG", NO_DELAY,
    stim, pop1,
    std_model, update_params, update_vars, {}, {},
    "ExpCond", ps_p, {},
    init_connectivity("OneToOne", {}))
stim.set_extra_global_param("spikeTimes", [0.0])
model.build()
model.load()
v = np.empty((2000, 10))
v_view = pop1.vars["V"].view
while model.t < 200.0:
    model.step_time()
    pop1.pull_var_from_device("V")
    
    v[model.timestep - 1,:]=v_view[:]
fig, axis = plt.subplots()
axis.plot(v)
plt.show()