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
#include <iostream>
#include <string>

namespace confu_soci
{
template <typename T> concept FusionSequence = requires (T a) { boost::fusion::at_c<0> (a); };

template <typename T> concept IsOptional = requires (T a) { a.has_value (); };

template <typename T> concept IsPrintable = requires (T t) { std::cout << t; };

template <typename T> concept IsPair = requires (T t) { t.second; };

template <class, template <class...> class> constexpr bool is_specialization = false;

template <template <class...> class T, class... Args> inline constexpr bool is_specialization<T<Args...>, T> = true;

template <class T> concept IsVector = is_specialization<T, std::vector>;

template <typename T> using is_adapted_struct = std::is_same<typename boost::fusion::traits::tag_of<T>::type, boost::fusion::struct_tag>;

template <typename T> concept IsFusionStruct = requires (T t) { T::self_type; };
}
#endif /* A071053C_A9C4_4BFF_A10C_45887320F653 */
