#include <string>

// only use constants (for generate_code_interface to work)

// model params
const float time_step = 0.05;
const int N_minicolumns = 10;
const int N_basket = 30;
const int N_pyramidal = 10;
const float wta_prob = 0.7;
const float lateral_prob = 0.25;
const int hyper_width = 2;
const int hyper_height = 1;
const float fMax = 50.0;
const float fDesired = 1.0;
const float epsilon = 0.001;
const float lateral_ampa_conductance = 0.00602; // 6.02 nS
const float lateral_nmda_conductance = 0.00122; // 1.22 nS
const float wta_gaba_conductance = 0.00602; // 1.22 nS
const float wta_ampa_conductance = 0.00602; // 1.22 nS
const float biasBaseGain = 0.05; // 50 pA

// simulation params
const float recall_time = 5000.0;
const float settle_time = 1000.0;
const float background_freq = 100.0;

// training params
const float training_freq = 150.0;
const int N_patterns = 4;
const int N_sequences = 4;
const float pattern_time = 200.0; // (ms) 10 attr/s
const float pattern_break = 0.0;  // (ms) 0 for sequence learning
const float sequence_break = 500.0;  // (ms) 0 for sequence learning
const int epochs = 5;             // number of training passes

const std::string minicolumn_basename = "M";
const std::string baskets_name = "_baskets";
const std::string hypercolumn_basename = "H";
const std::string input_basename = "_input";

const std::string wta_ampa_name = "_wta_ampa";
const std::string wta_gaba_name = "_wta_gaba";
const std::string lateral_ampa_name = "_lateral_ampa";
const std::string lateral_nmda_name = "_lateral_nmda";
