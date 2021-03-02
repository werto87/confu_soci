#ifndef B7C2FAB9_F015_4288_92A6_13D53DA86731
#define B7C2FAB9_F015_4288_92A6_13D53DA86731
#include "confu_soci/concept.hxx"
#include "soci/soci.h"
#include "soci/sqlite3/soci-sqlite3.h"
#include <boost/algorithm/algorithm.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/split.hpp>
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
#include <boost/lexical_cast.hpp>
#include <boost/mpl/for_each.hpp>
#include <boost/mpl/if.hpp>
#include <boost/mpl/range_c.hpp>
#include <boost/optional.hpp>
#include <boost/optional/optional.hpp>
#include <boost/optional/optional_io.hpp>
#include <boost/type_index.hpp>
#include <boost/type_traits/remove_reference.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <exception>
#include <iostream>
#include <istream>
#include <magic_enum.hpp>
#include <ostream>
#include <soci/boost-fusion.h>
#include <soci/boost-gregorian-date.h>
#include <soci/boost-optional.h>
#include <soci/boost-tuple.h>
#include <sstream>
#include <string>
#include <tuple>
#include <type_traits>
namespace soci
{
template <typename E> struct type_conversion<E, typename std::enable_if<std::is_enum<E>::value>::type>
{
  typedef std::string base_type;
  static void
  from_base (std::string const &v, indicator /* ind */, E &p)
  {
    p = magic_enum::enum_cast<E> (v).value ();
  }

  static void
  to_base (const E &p, std::string &v, indicator &ind)
  {
    v = std::string{ magic_enum::enum_name<E> (p) };
    ind = i_ok;
  }
};
}

namespace confu_soci
{

template <typename T>
std::string
// only use this if your type is a simple user defined type with not template class something like namespace::MyType and you want to get MyType
// does not work with templated types or something like std::string which is std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > and the result will be allocator<char> >
typeNameWithOutNamespace (T const &)
{
  auto fullName = std::vector<std::string>{};
  auto typeWithNamespace = boost::typeindex::type_id<T> ().pretty_name ();
  boost::algorithm::split (fullName, typeWithNamespace, boost::is_any_of ("::"));
  return fullName.back ();
}

template <typename T>
std::string
typeName (T const &)
{
  return boost::typeindex::type_id<T> ().pretty_name ();
}

template <typename T> concept printable = requires (T t)
{
  {
    std::cout << t
  }
  ->std::same_as<std::ostream &>;
};

template <FusionSequence T>
std::string
structAsString (T const &structToPrint)
{
  auto _structAsString = std::stringstream{};
  _structAsString << typeName (structToPrint) << std::endl;
  boost::fusion::for_each (boost::mpl::range_c<unsigned, 0, boost::fusion::result_of::size<T>::value> (), [&] (auto index) {
    if constexpr (std::is_enum<typename boost::fusion::result_of::value_at_c<T, index>::type>::value)
      {
        _structAsString << "Name: " << boost::fusion::extension::struct_member_name<T, index>::call () << " Value: " << magic_enum::enum_name (boost::fusion::at_c<index> (structToPrint)) << std::endl;
      }
    else if constexpr (requires { _structAsString << boost::fusion::at_c<index> (structToPrint); })
      {
        _structAsString << "Name: " << boost::fusion::extension::struct_member_name<T, index>::call () << " Value: " << boost::fusion::at_c<index> (structToPrint) << std::endl;
      }
    else
      {
        _structAsString << "Member: " << boost::fusion::extension::struct_member_name<T, index>::call () << " Type: " << structAsString (boost::fusion::at_c<index> (structToPrint));
      }
  });
  return _structAsString.str ();
}

// about the shouldGenerateId option
// id is string:
// generates a uuid using boost uuid
// id is integral
// uses highest id +1
// !!! shouldGenerateId is not thread save !!!
//
template <FusionSequence T>
auto
insertStruct (soci::session &sql, T const &structToInsert, bool foreignKeyConstraints = false, bool shouldGenerateId = false)
{
  auto ss = std::stringstream{};
  ss << "INSERT INTO " << typeNameWithOutNamespace (structToInsert) << '(';
  boost::fusion::for_each (boost::mpl::range_c<int, 0, boost::fusion::result_of::size<T>::value> (), [&] (auto index) {
    ss << boost::fusion::extension::struct_member_name<T, index>::call ();
    if (index < boost::fusion::size (structToInsert) - 1)
      {
        ss << ',';
      }
  });
  ss << ") VALUES(";
  auto id = std::remove_reference_t<decltype (boost::fusion::at_c<0> (structToInsert))>{};
  boost::fusion::for_each (boost::mpl::range_c<int, 0, boost::fusion::result_of::size<T>::value> (), [&] (auto index) {
    if (index == 0)
      {
        if constexpr (std::is_integral_v<std::remove_reference_t<decltype (boost::fusion::at_c<0> (structToInsert))> >)
          {
            if (shouldGenerateId)
              {
                auto newId = 0ll;
                if (sql.get_last_insert_id (typeNameWithOutNamespace (structToInsert), newId))
                  {
                    id = newId + 1;
                    ss << id;
                  }
                else
                  {
                    throw soci::soci_error{ "get_last_insert_id exception" };
                  }
              }
            else
              {
                id = boost::fusion::at_c<index> (structToInsert);
                ss << id;
              }
          }
        else
          {
            if (shouldGenerateId)
              {
                static boost::uuids::random_generator generator;
                id = boost::lexical_cast<std::string> (generator ());
              }
            else
              {
                id = boost::fusion::at_c<index> (structToInsert);
              }
            ss << "'" << id << "'";
          }
      }
    else if constexpr (std::is_integral_v<std::remove_reference_t<decltype (boost::fusion::at_c<0> (structToInsert))> >)
      {
        ss << boost::fusion::at_c<index> (structToInsert);
      }
    else
      {
        ss << "'" << boost::fusion::at_c<index> (structToInsert) << "'";
      }
    if (index < boost::fusion::size (structToInsert) - 1)
      {
        ss << ',';
      }
  });
  ss << ')';
  auto foreignKeysEnabled = 0;
  sql << "PRAGMA foreign_keys;", soci::into (foreignKeysEnabled);
  try
    {
      if (foreignKeyConstraints && foreignKeysEnabled == 0)
        {
          sql << "PRAGMA foreign_keys = ON;";
        }

      sql << ss.str ();
      if (foreignKeyConstraints && foreignKeysEnabled == 0)
        {
          sql << "PRAGMA foreign_keys = OFF;";
        }
    }
  catch (std::exception const &e)
    {
      if (foreignKeyConstraints && foreignKeysEnabled == 0)
        {
          sql << "PRAGMA foreign_keys = OFF;";
        }
      throw;
    }
  if constexpr (std::is_integral_v<std::remove_reference_t<decltype (boost::fusion::at_c<0> (structToInsert))> >)
    {
      return id;
    }
  else
    {
      return id;
    }
}

template <FusionSequence T>
void
updateStruct (soci::session &sql, T const &structToUpdate, bool foreignKeyConstraints = false)
{
  auto ss = std::stringstream{};
  ss << "UPDATE " << typeNameWithOutNamespace (structToUpdate) << " SET ";
  boost::fusion::for_each (boost::mpl::range_c<int, 0, boost::fusion::result_of::size<T>::value> (), [&] (auto index) {
    ss << boost::fusion::extension::struct_member_name<T, index>::call ();
    ss << "=:";
    ss << boost::fusion::extension::struct_member_name<T, index>::call ();
    if (index < boost::fusion::size (structToUpdate) - 1)
      {
        ss << ',';
      }
  });
  ss << " WHERE ";
  ss << boost::fusion::extension::struct_member_name<T, 0>::call ();
  ss << '=';
  if constexpr (std::is_integral_v<std::remove_reference_t<decltype (boost::fusion::at_c<0> (structToUpdate))> >)
    {
      ss << boost::fusion::at_c<0> (structToUpdate);
    }
  else
    {
      ss << "'";
      ss << boost::fusion::at_c<0> (structToUpdate);
      ss << "'";
    }
  // sql << ss.str (), soci::use (structToUpdate);
  soci::statement st = (sql.prepare << ss.str (), soci::use (structToUpdate));
  if (foreignKeyConstraints)
    {
      sql << "PRAGMA foreign_keys = ON;";
    }
  st.execute (true);
  if (foreignKeyConstraints)
    {
      sql << "PRAGMA foreign_keys = OFF;";
    }
  if (st.get_affected_rows () == 0) throw soci::soci_error{ "could not find struct to update\n" + structAsString (structToUpdate) };
}

template <typename T>
void
upsertStruct (soci::session &sql, T const &structToInsert, bool foreignKeyConstraints = false)
{
  try
    {
      insertStruct (sql, structToInsert);
    }
  catch (std::exception const &)
    {
      updateStruct (sql, structToInsert);
    }
}

template <typename T>
boost::optional<T>
findStruct (soci::session &sql, std::string const &columnName, auto const &value)
{
  auto tableName = typeNameWithOutNamespace (T{});
  auto ss = std::stringstream{};
  ss << "SELECT * FROM " << tableName << " WHERE " << columnName << '=' << "'" << value << "'";
  auto result = T{};
  sql << ss.str (), soci::into (result);
  if (sql.got_data ())
    {
      return result;
    }
  else
    {
      return boost::none;
    }
}

bool doesTableExist (soci::session &sql, std::string const &tableName);

template <typename T>
bool
doesTableExist (soci::session &sql)
{
  return doesTableExist (sql, typeNameWithOutNamespace (T{}));
}

template <FusionSequence T>
void
createTableForStruct (soci::session &sql, std::vector<std::tuple<std::string, std::string, std::string> > const &foreignKeys = {})
{
  auto tableName = typeNameWithOutNamespace (T{});
  if (doesTableExist (sql, tableName)) throw soci::soci_error{ tableName + " table already exists" };
  soci::ddl_type ddl = sql.create_table (tableName);
  boost::fusion::for_each (boost::mpl::range_c<int, 0, boost::fusion::result_of::size<T>::value> (), [&] (auto index) {
    auto memberName = boost::fusion::extension::struct_member_name<T, index>::call ();
    auto memberTypeAsString = typeName (boost::fusion::at_c<index> (T{}));
    const static auto sociTypeMapping = std::map<std::string, soci::data_type>{ { "std::string", soci::data_type::dt_string }, { "boost::optional<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > >", soci::data_type::dt_string }, { "std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >", soci::data_type::dt_string }, { "date", soci::data_type::dt_date }, { "double", soci::data_type::dt_double }, { "int", soci::data_type::dt_integer }, { "boost::optional<long>", soci::data_type::dt_long_long }, { "long", soci::data_type::dt_long_long }, { "unsigned long", soci::data_type::dt_unsigned_long_long }, { "blob", soci::data_type::dt_blob }, { "xml", soci::data_type::dt_xml } };
    auto databaseType = soci::data_type::dt_string;
    try
      {
        databaseType = sociTypeMapping.at (memberTypeAsString);
      }
    catch (std::exception const &e)
      {
        databaseType = soci::data_type::dt_string;
      }
    ddl.column (memberName, databaseType);
  });
  auto primaryKey = boost::fusion::extension::struct_member_name<T, 0>::call ();
  ddl.primary_key (tableName + "_" + primaryKey, primaryKey);
  for (auto const &[column, foreignTabel, foreignColumn] : foreignKeys)
    {
      if (column.empty ())
        {
          throw soci::soci_error{ "can not create " + tableName + " because the forign key option column is empty. CHECK first element in forign key triplet" };
        }
      else if (foreignTabel.empty ())
        {
          throw soci::soci_error{ "can not create " + tableName + " because the forign key option foreignTabel is empty. CHECK second element in forign key triplet" };
        }
      else if (foreignColumn.empty ())
        {
          throw soci::soci_error{ "can not create " + tableName + " because the forign key option foreignColumn is empty. CHECK third element in forign key triplet" };
        }
      ddl.foreign_key (tableName + "_" + column, column, foreignTabel, foreignColumn);
    }
}

template <typename T>
void
dropTableForStruct (soci::session &sql)
{
  if (not doesTableExist (sql, typeNameWithOutNamespace (T{}))) throw soci::soci_error{ typeNameWithOutNamespace (T{}) + " table does not exist" };
  sql << "DROP TABLE " + typeNameWithOutNamespace (T{});
}

template <FusionSequence fusionSequenceWithTypes>
void
createTables (soci::session &sql)
{
  boost::mpl::for_each<fusionSequenceWithTypes, boost::mpl::make_identity<boost::mpl::_1> > ([&] (auto arg) {
    using T = typename decltype (arg)::type;
    createTableForStruct<T> (sql);
  });
}

template <FusionSequence fusionSequenceWithTypes>
std::vector<std::string>
dropTables (soci::session &sql)
{
  auto result = std::vector<std::string>{};
  boost::mpl::for_each<fusionSequenceWithTypes, boost::mpl::make_identity<boost::mpl::_1> > ([&] (auto arg) {
    using T = typename decltype (arg)::type;
    try
      {
        dropTableForStruct<T> (sql);
      }
    catch (std::exception const &e)
      {
        result.push_back (typeName (T{}));
      }
  });
  return result;
}

}
#endif /* B7C2FAB9_F015_4288_92A6_13D53DA86731 */
