#include <string>

// only use constants (for generate_code_interface to work)

// model params
const float time_step = 0.1;
const int N_minicolumns = 10;
const int N_basket = 30;
const int N_pyramidal = 10;
const float wta_prob = 0.7;
const float lateral_prob = 0.25;
const int hyper_width = 2;
const int hyper_height = 2;

// simulation params
const int record_buffer = 1000;
const float recall_time = 10000.0;
const float background_freq = 350.0;

// training params
const float training_freq = 150.0;
const float pattern_time = 200.0; // (ms) 10 attr/s
const float pattern_break = 0.0; // (ms) 0 for sequence learning
const int epochs = 10; // number of training passes








const std::string minicolumn_basename = "M";
const std::string baskets_name = "baskets";
const std::string hypercolumn_basename = "H";
const std::string input_basename = "input";

const std::string wta_ampa_name = "wta_ampa";
const std::string wta_gaba_name = "wta_gaba";
const std::string lateral_ampa_name = "lateral_ampa";
const std::string lateral_nmda_name = "lateral_nmda";
