#include <iostream>
#include <random>
#include <vector>
#include <tuple>
#include <array>
#include <string>

#include <boost/detail/lightweight_test.hpp>

#include "../genep/genep.hpp"

typedef std::pair<std::tuple<double, double>, std::tuple<double> > data_type;

inline
double secret_expression(double param1, double param2)
{
    return (10 * param1 - param2) / (param2 + param1);
}

template<class Engine>
std::vector<data_type> get_1000_data(Engine& engine)
{
    std::uniform_real_distribution<double> dst(0, 1000);
    std::vector<data_type> data;
    data.reserve(1000);
    for(int i=0; i<1000; ++i){
        auto x1 = dst(engine);
        auto x2 = dst(engine);
        auto input = std::make_tuple(x1, x2);
        auto output = std::make_tuple(secret_expression(x1, x2));
        data.emplace_back(input, output);
    }
    return data;
}

template<class Result, class Engine>
void validate(Result const& result, Engine& engine)
{
    auto validate_data = get_1000_data(engine);
    for(auto const& validater : validate_data){
        // GP's output is correct or not
        BOOST_TEST_EQ( validater.second[0],
                       result.value(validater.first[0], validater.first[1]) );
    }
}

int main()
{
    std::random_device device;
    std::mt19937 engine(device());

    genep::population< genep::input<double, double>,
                       genep::output<double> > population;
    population.set_training_data(get_1000_data());
    while(population.fitness() > 10e-7){
        population.next_generation();
    }
    auto result = population.most_suitable_individual()[0];
    std::cout << "f(x0) = " << result;

    return 0;
}
