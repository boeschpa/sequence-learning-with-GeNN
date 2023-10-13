#include "initSparseConnectivitySnippet.h"

// maybe based on original implementation but give distribution to snippet
class MatrixConnectivity : public InitSparseConnectivitySnippet::Base
{
    DECLARE_SNIPPET(MatrixConnectivity, 1);
    
    SET_ROW_BUILD_STATE_VARS({{"column", "unsigned int", 0}});
    SET_ROW_BUILD_CODE(
        // add synapse from id_pre to target
        "if ($(matrix)[column][$(id_pre)]==1){\n"
        "   $(addSynapse, column);\n"
        "}\n"
        "column++;\n"
        "if(column > (unsigned int)$(numPost)) {\n}"
        "   $(endRow);\n"
        "}\n");
        
    SET_PARAM_NAMES({"matrix"});
    SET_CALC_MAX_ROW_LENGTH_FUNC(
        [](unsigned int numPre, unsigned int numPost, const std::vector<double> &pars)
        {
            return (unsigned int)pars[0];
        });
    SET_CALC_MAX_COL_LENGTH_FUNC(
        [](unsigned int numPre, unsigned int numPost, const std::vector<double> &pars)
        {
            return (unsigned int)pars[0];
        });

};
IMPLEMENT_SNIPPET(MatrixConnectivity);

