#include <string>

// model params
const float time_step = 0.1;
const int N_minicolumns = 10;
const int N_basket = 30;
const int N_pyramidal = 10;
const float wta_prob = 0.7;
const float lateral_prob = 0.75;
const int hyper_width = 2;
const int hyper_height = 1;

const float sim_time = 2000.0;
const float background_freq = 350.0;

const std::string minicolumn_basename = "M";
const std::string baskets_name = "baskets";
const std::string hypercolumn_basename = "H";
const std::string input_basename = "input";

const std::string wta_ampa_name = "wta_ampa";
const std::string wta_gaba_name = "wta_gaba";
const std::string lateral_ampa_name = "lateral_ampa";
const std::string lateral_nmda_name = "lateral_nmda";
