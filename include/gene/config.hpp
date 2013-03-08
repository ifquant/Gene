#if !defined GENE_CONFIG_HPP_INCLUDED
#define      GENE_CONFIG_HPP_INCLUDED

#include <random>

namespace gene {

namespace config {

    static std::mt19937 random_engine(std::random_device{}());

} // namespace config
} // namespace gene

#endif    // GENE_CONFIG_HPP_INCLUDED
