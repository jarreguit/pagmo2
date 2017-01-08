#define BOOST_TEST_MODULE pagmo_rastrigin_test
#include <boost/test/included/unit_test.hpp>

#include <boost/lexical_cast.hpp>
#include <exception>
#include <iostream>
#include <string>

#include "../include/detail/constants.hpp"
#include "../include/problem.hpp"
#include "../include/problems/rastrigin.hpp"
#include "../include/problems/null_problem.hpp"
#include "../include/types.hpp"

using namespace pagmo;

BOOST_AUTO_TEST_CASE(rastrigin_test)
{
    // Problem construction
    rastrigin ras1{1u};
    rastrigin ras5{5u};
    BOOST_CHECK_THROW(rastrigin{0u}, std::invalid_argument);
    BOOST_CHECK_NO_THROW(problem{rastrigin{2u}});
    // Pick a few reference points
    vector_double x1 = {1.};
    vector_double x5 = {1., 1., 1., 1., 1.};
    // Fitness test
    BOOST_CHECK((ras1.fitness(x1) == vector_double{1.}));
    BOOST_CHECK((ras5.fitness(x5) == vector_double{5.}));
    // Gradient test
    auto g1 = ras1.gradient(x1);
    auto g5 = ras5.gradient(x5);
    for (decltype(g1.size()) i = 0u; i < g1.size(); ++i) {
        BOOST_CHECK_CLOSE(g1[i], 2., 1e-12);
    }
    for (decltype(g5.size()) i = 0u; i < g5.size(); ++i) {
        BOOST_CHECK_CLOSE(g5[i], 2., 1e-12);
    }
    // Hessians test
    auto h1 = ras1.hessians(x1);
    auto h5 = ras5.hessians(x5);
    for (decltype(h1[0].size()) i = 0u; i < h1[0].size(); ++i) {
        BOOST_CHECK_CLOSE(h1[0][i], 2. + 4 * detail::pi() * detail::pi() * 10, 1e-12);
    }
    for (decltype(h5[0].size()) i = 0u; i < h5[0].size(); ++i) {
        BOOST_CHECK_CLOSE(h5[0][i], 2. + 4 * detail::pi() * detail::pi() * 10, 1e-12);
    }
    // Hessian sparsity test
    BOOST_CHECK((ras1.hessians_sparsity() == std::vector<sparsity_pattern>{{{0,0}}}));
    BOOST_CHECK((ras5.hessians_sparsity() == std::vector<sparsity_pattern>{{{0,0},{1,1},{2,2},{3,3},{4,4}}}));
    // Bounds Test
    BOOST_CHECK((ras1.get_bounds() == std::pair<vector_double, vector_double>{{-5.12},{5.12}}));
    // Name and extra info tests
    BOOST_CHECK(ras5.get_name().find("Rastrigin") != std::string::npos);
    // Best known test
    auto x_best = ras5.best_known();
    BOOST_CHECK((x_best == vector_double{0., 0., 0., 0., 0.}));
}

BOOST_AUTO_TEST_CASE(rastrigin_serialization_test)
{
    problem p{rastrigin{4u}};
    // Call objfun to increase the internal counters.
    p.fitness({1.,1.,1.,1.});
    p.gradient({1.,1.,1.,1.});
    p.hessians({1.,1.,1.,1.});
    // Store the string representation of p.
    std::stringstream ss;
    auto before = boost::lexical_cast<std::string>(p);
    // Now serialize, deserialize and compare the result.
    {
    cereal::JSONOutputArchive oarchive(ss);
    oarchive(p);
    }
    // Change the content of p before deserializing.
    p = problem{null_problem{}};
    {
    cereal::JSONInputArchive iarchive(ss);
    iarchive(p);
    }
    auto after = boost::lexical_cast<std::string>(p);
    BOOST_CHECK_EQUAL(before, after);
}