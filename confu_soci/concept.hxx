#ifndef A071053C_A9C4_4BFF_A10C_45887320F653
#define A071053C_A9C4_4BFF_A10C_45887320F653
#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <boost/fusion/adapted/struct/define_struct.hpp>
#include <boost/fusion/algorithm/query/count.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/algorithm.hpp>
#include <boost/fusion/include/at.hpp>
#include <boost/fusion/include/count.hpp>
#include <boost/fusion/include/define_struct.hpp>
#include <boost/fusion/sequence/intrinsic/at.hpp>
#include <boost/fusion/sequence/intrinsic_fwd.hpp>
#include <concepts>
#include <cstddef>
#include <string>

template <typename T> concept FusionSequence = requires (T a) { boost::fusion::at_c<0> (a); };

#endif /* A071053C_A9C4_4BFF_A10C_45887320F653 */
