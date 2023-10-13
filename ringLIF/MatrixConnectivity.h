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
        "if(column > (unsigned int)$(numPos)) {\n}"
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






//----------------------------------------------------------------------------
// InitSparseConnectivitySnippet::FixedNumberTotalWithReplacement
//----------------------------------------------------------------------------
//! Initialises connectivity with a total number of random synapses.
//! The first stage in using this connectivity is to determine how many of the total synapses end up in each row.
//! This can be determined by sampling from the multinomial distribution. However, this operation cannot be
//! efficiently parallelised so must be performed on the host and the result passed as an extra global parameter array.
/*! Once the length of each row is determined, the postsynaptic targets of the synapses can be initialised in parallel
    by sampling from the discrete uniform distribution. However, to sample connections in ascending order, we sample
    from the 1st order statistic of the uniform distribution -- Beta[1, Npost] -- essentially the next smallest value.
    In this special case this is equivalent to the exponential distribution which can be sampled in constant time using the inversion method.*/
class FixedNumberTotalWithReplacement : public Base
{
public:
    DECLARE_SNIPPET(InitSparseConnectivitySnippet::FixedNumberTotalWithReplacement, 1);

    SET_ROW_BUILD_CODE(
        "if(c == 0) {\n"
        "   $(endRow);\n"
        "}\n"
        "const scalar u = $(gennrand_uniform);\n"
        "x += (1.0 - x) * (1.0 - pow(u, 1.0 / (scalar)c));\n"
        "unsigned int postIdx = (unsigned int)(x * $(num_post));\n"
        "postIdx = (postIdx < $(num_post)) ? postIdx : ($(num_post) - 1);\n"
        "$(addSynapse, postIdx + $(id_post_begin));\n"
        "c--;\n");
    SET_ROW_BUILD_STATE_VARS({{"x", "scalar", 0.0},{"c", "unsigned int", "$(preCalcRowLength)[($(id_pre) * $(num_threads)) + $(id_thread)]"}});

    SET_PARAM_NAMES({"total"});
    SET_EXTRA_GLOBAL_PARAMS({{"preCalcRowLength", "uint16_t*"}})

    SET_HOST_INIT_CODE(
        "// Allocate pre-calculated row length array\n"
        "$(allocatepreCalcRowLength, $(num_pre) * $(num_threads));\n"
        "// Calculate row lengths\n"
        "const size_t numPostPerThread = ($(num_post) + $(num_threads) - 1) / $(num_threads);\n"
        "const size_t leftOverNeurons = $(num_post) % numPostPerThread;\n"
        "size_t remainingConnections = $(total);\n"
        "size_t matrixSize = (size_t)$(num_pre) * (size_t)$(num_post);\n"
        "uint16_t *subRowLengths = $(preCalcRowLength);\n"
        "// Loop through rows\n"
        "for(size_t i = 0; i < $(num_pre); i++) {\n"
        "    const bool lastPre = (i == ($(num_pre) - 1));\n"
        "    // Loop through subrows\n"
        "    for(size_t j = 0; j < $(num_threads); j++) {\n"
        "        const bool lastSubRow = (j == ($(num_threads) - 1));\n"
        "        // If this isn't the last sub-row of the matrix\n"
        "        if(!lastPre || ! lastSubRow) {\n"
        "            // Get length of this subrow\n"
        "            const unsigned int numSubRowNeurons = (leftOverNeurons != 0 && lastSubRow) ? leftOverNeurons : numPostPerThread;\n"
        "            // Calculate probability\n"
        "            const double probability = (double)numSubRowNeurons / (double)matrixSize;\n"
        "            // Create distribution to sample row length\n"
        "            std::binomial_distribution<size_t> rowLengthDist(remainingConnections, probability);\n"
        "            // Sample row length;\n"
        "            const size_t subRowLength = rowLengthDist($(rng));\n"
        "            // Update counters\n"
        "            remainingConnections -= subRowLength;\n"
        "            matrixSize -= numSubRowNeurons;\n"
        "            // Add row length to array\n"
        "            assert(subRowLength < std::numeric_limits<uint16_t>::max());\n"
        "            *subRowLengths++ = (uint16_t)subRowLength;\n"
        "        }\n"
        "    }\n"
        "}\n"
        "// Insert remaining connections into last sub-row\n"
        "*subRowLengths = (uint16_t)remainingConnections;\n"
        "// Push populated row length array\n"
        "$(pushpreCalcRowLength, $(num_pre) * $(num_threads));\n");

    SET_CALC_MAX_ROW_LENGTH_FUNC(
        [](unsigned int numPre, unsigned int numPost, const std::vector<double> &pars)
        {
            // Calculate suitable quantile for 0.9999 change when drawing numPre times
            const double quantile = pow(0.9999, 1.0 / (double)numPre);

            // There are numConnections connections amongst the numPre*numPost possible connections.
            // Each of the numConnections connections has an independent p=float(numPost)/(numPre*numPost)
            // probability of being selected and the number of synapses in the sub-row is binomially distributed
            return binomialInverseCDF(quantile, (unsigned int)pars[0], (double)numPost / ((double)numPre * (double)numPost));
        });

    SET_CALC_MAX_COL_LENGTH_FUNC(
        [](unsigned int numPre, unsigned int numPost, const std::vector<double> &pars)
        {
            // Calculate suitable quantile for 0.9999 change when drawing numPost times
            const double quantile = pow(0.9999, 1.0 / (double)numPost);

            // There are numConnections connections amongst the numPre*numPost possible connections.
            // Each of the numConnections connections has an independent p=float(numPre)/(numPre*numPost)
            // probability of being selected and the number of synapses in the sub-row is binomially distributed
            return binomialInverseCDF(quantile, (unsigned int)pars[0], (double)numPre / ((double)numPre * (double)numPost));
        });
};
