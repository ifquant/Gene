#if !defined GENE_CONFIG_HPP_INCLUDED
#define      GENE_CONFIG_HPP_INCLUDED

#include <random>
#include <cstddef>

namespace gene {

namespace config {

    static std::mt19937 random_engine(std::random_device{}());
    static std::size_t indent_width = 4;
    static std::size_t random_tree_depth = 4;

} // namespace config
} // namespace gene

#endif    // GENE_CONFIG_HPP_INCLUDED
