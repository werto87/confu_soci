#include <boost/algorithm/algorithm.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <boost/fusion/adapted/struct/define_struct.hpp>
#include <boost/fusion/algorithm/query/count.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/algorithm.hpp>
#include <boost/fusion/include/count.hpp>
#include <boost/fusion/include/define_struct.hpp>
#include <boost/mpl/list.hpp>
#include <boost/mpl/range_c.hpp>
#include <boost/optional.hpp>
#include <boost/optional/optional_io.hpp>
#include <boost/serialization/optional.hpp>
#include <boost/type_index.hpp>
#include <catch2/catch.hpp>
#include <confu_soci/convenienceFunctionForSoci.hxx>
#include <cstdint>
#include <exception>
#include <magic_enum/magic_enum.hpp>
#include <soci/exchange-traits.h>
#include <soci/session.h>
#include <soci/soci.h>
#include <soci/sqlite3/soci-sqlite3.h>
#include <soci/use-type.h>
#include <soci/values-exchange.h>
#include <string>
#include <test/constant.hxx>
#include <test/testHelper.hxx>

enum struct BoardElementType : char
{
  Plain,
  Hill,
  Valley
};
enum class Direction : char
{
  UpLeft,
  Up,
  UpRight,
  Left,
  Nothing,
  Right,
  DownLeft,
  Down,
  DownRight
};
namespace test
{
struct BoardElement
{
  unsigned long id{};
  boost::optional<std::string> playerId{};
  BoardElementType boardElementType{};
  float movementCostToMoveThroughVerticalOrHorizontal = 1.f;
};
}

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4003)
#endif

BOOST_FUSION_ADAPT_STRUCT (test::BoardElement, (unsigned long, id) (boost::optional<std::string>, playerId) (BoardElementType, boardElementType) (float, movementCostToMoveThroughVerticalOrHorizontal))

BOOST_FUSION_DEFINE_STRUCT ((test), IdAndOptional, (unsigned long, id) (boost::optional<std::string>, optionalText))

BOOST_FUSION_DEFINE_STRUCT ((test), Player, (std::string, playerId) (Direction, viewDirection) (Direction, moveDirection) (double, viewFieldsize))

BOOST_FUSION_DEFINE_STRUCT ((test), EasyClass, (std::string, playerId) (double, points))

BOOST_FUSION_DEFINE_STRUCT ((test), AllSupportedTypes, (std::string, stringType) (double, doubleType) (int, intType) (unsigned long, unsignedLongType) (long, longType))

BOOST_FUSION_DEFINE_STRUCT ((test), TableWithForignKeyToEasyClass, (std::string, id) (double, easyClassId))

BOOST_FUSION_DEFINE_STRUCT ((), MyClass, (int, someInt))
BOOST_FUSION_DEFINE_STRUCT ((test), NestedClass, (int, id) (MyClass, myClass) (MyClass, yourClass))
BOOST_FUSION_DEFINE_STRUCT ((), Board, (std::string, id) (std::string, gameId))
BOOST_FUSION_DEFINE_STRUCT ((), Game, (std::string, id))
BOOST_FUSION_DEFINE_STRUCT ((), MyVector, (unsigned long, id) (std::vector<uint8_t>, someVector))
BOOST_FUSION_DEFINE_STRUCT ((), MyVectorStringId, (std::string, id) (std::vector<uint8_t>, someVector))
BOOST_FUSION_DEFINE_STRUCT ((), MyVectorStringIdAndEnum, (std::string, id) (std::vector<uint8_t>, someVector) (Direction, direction))
#ifdef _MSC_VER
#pragma warning(pop)
#endif
namespace soci
{
template <> struct type_conversion<MyClass>
{
  typedef values base_type;

  static void
  from_base (base_type const &v, indicator /* ind */, MyClass &p)
  {
    boost::fusion::for_each (boost::mpl::range_c<int, 0, boost::fusion::result_of::size<MyClass>::value> (), [&] (auto index) {
      //
      boost::fusion::at_c<index> (p) = v.get<std::remove_reference_t<decltype (boost::fusion::at_c<index> (p))> > (boost::fusion::extension::struct_member_name<MyClass, index>::call ());
    });
  }

  static void
  to_base (const MyClass &p, base_type &v, indicator &ind)
  {
    boost::fusion::for_each (boost::mpl::range_c<int, 0, boost::fusion::result_of::size<MyClass>::value> (), [&] (auto index) {
      //
      v.set (boost::fusion::extension::struct_member_name<MyClass, index>::call (), boost::fusion::at_c<index> (p));
    });
    ind = i_ok;
  }
};
}

typedef boost::mpl::list<test::Player, test::BoardElement> tables;

using namespace confu_soci;

namespace test
{
TEST_CASE ("type name with out namespace") { REQUIRE (typeNameWithOutNamespace (Player{}) == "Player"); }
TEST_CASE ("type name with namespace") { REQUIRE (typeName (Player{}) == "test::Player"); }
TEST_CASE ("generateInsert") { REQUIRE (generateInsert (EasyClass{}) == "INSERT INTO EasyClass(playerId,points) VALUES(:playerId,:points)"); }

TEST_CASE ("doesTableExists")
{
  resetTestDatabase ();
  soci::session sql (soci::sqlite3, pathToTestDatabase);
  SECTION ("table does exist")
  {
    createTableForStruct<EasyClass> (sql);
    REQUIRE (doesTableExist<EasyClass> (sql));
  }
  SECTION ("table does not exist") { REQUIRE (not doesTableExist<EasyClass> (sql)); }
}

SCENARIO ("create a table with createTableForStruct", "[createTableForStruct]")
{
  GIVEN ("a connection to a database where the table does not exist")
  {
    resetTestDatabase ();
    soci::session sql (soci::sqlite3, pathToTestDatabase);
    REQUIRE (not doesTableExist<AllSupportedTypes> (sql));
    WHEN ("the table is created")
    {
      createTableForStruct<AllSupportedTypes> (sql);
      THEN ("table exists")
      {
        REQUIRE (doesTableExist<AllSupportedTypes> (sql));
      }
      THEN ("columns have correct type")
      {
        REQUIRE(getColumnDataType<AllSupportedTypes> (sql, 0)== soci::data_type::dt_string);
        REQUIRE(getColumnDataType<AllSupportedTypes> (sql, 1)== soci::data_type::dt_double);
        REQUIRE(getColumnDataType<AllSupportedTypes> (sql, 2)== soci::data_type::dt_integer);
        REQUIRE(getColumnDataType<AllSupportedTypes> (sql, 3)== soci::data_type::dt_unsigned_long_long);
        REQUIRE(getColumnDataType<AllSupportedTypes> (sql, 4)== soci::data_type::dt_long_long);
      }
    }
  }
  GIVEN ("a connection to a database where the table exist")
  {
    resetTestDatabase ();
    soci::session sql (soci::sqlite3, pathToTestDatabase);
    createTableForStruct<EasyClass> (sql);
    WHEN ("table is in database")
    {
      REQUIRE (doesTableExist<EasyClass> (sql));
      THEN ("createTableForStruct throws exception")
      {
        try
          {
            createTableForStruct<EasyClass> (sql);
          }
        catch (soci::soci_error &e)
          {
            REQUIRE (std::string{ "EasyClass table already exists" } == e.what ());
          }
      }
    }
  }
  GIVEN ("a connection to a database where the table does not exist")
  {
    resetTestDatabase ();
    soci::session sql (soci::sqlite3, pathToTestDatabase);
    WHEN ("the table is created with foreign keys")
    {
      //
      createTableForStruct<EasyClass> (sql);
      createTableForStruct<TableWithForignKeyToEasyClass> (sql, { { "easyClassId", "EasyClass", "id" } });
      THEN ("the tables do exist")
      {
        REQUIRE (doesTableExist<TableWithForignKeyToEasyClass> (sql));
        REQUIRE (doesTableExist<EasyClass> (sql));
      }
    }
  }
}

SCENARIO ("insert struct in database with insertStruct", "[insertStruct]")
{
  GIVEN ("a connection to a database where the table exist")
  {
    resetTestDatabase ();
    soci::session sql (soci::sqlite3, pathToTestDatabase);
    createTableForStruct<EasyClass> (sql);
    REQUIRE (doesTableExist<EasyClass> (sql));
    WHEN ("record gets inserted in table")
    {
      REQUIRE_NOTHROW (insertStruct (sql, EasyClass{ "222", 42 }));
      THEN ("the record gets added to the table") { REQUIRE (findStruct<EasyClass> (sql, "playerId", "222").has_value ()); }
    }
  }
  GIVEN ("a connection to a database where the table exists and has a record")
  {
    resetTestDatabase ();
    soci::session sql (soci::sqlite3, pathToTestDatabase);
    createTableForStruct<EasyClass> (sql);
    double oldValue = 42;
    insertStruct (sql, EasyClass{ "222", oldValue });
    REQUIRE (doesTableExist<EasyClass> (sql));
    WHEN ("record gets inserted in table")
    {
      double newValue = 1337;
      try
        {
          insertStruct (sql, EasyClass{ "222", newValue });
          FAIL ("should throw exception but did not throw exception");
        }
      catch (soci::soci_error const &e)
        {
          THEN ("throws exception") { REQUIRE (soci::soci_error::error_category::constraint_violation == e.get_error_category ()); }
        }
    }
  }
  GIVEN ("a connection to a database where the tables exists whit foreign key constraints")
  {
    resetTestDatabase ();
    soci::session sql (soci::sqlite3, pathToTestDatabase);
    confu_soci::createTableForStruct<Board> (sql, { { "gameId", "Game", "id" } });
    confu_soci::createTableForStruct<Game> (sql);
    REQUIRE (doesTableExist<Board> (sql));
    REQUIRE (doesTableExist<Game> (sql));
    WHEN ("record gets inserted in table with foreign key constraints enabled")
    {
      insertStruct (sql, Game{ "game1" }, true);
      insertStruct (sql, Board{ "board1", "game1" }, true);
      THEN ("record is in table") { REQUIRE (findStruct<Board> (sql, "gameId", std::string{ "game1" }).has_value ()); }
    }
  }
  GIVEN ("a connection to a database where the tables exists")
  {
    resetTestDatabase ();
    soci::session sql (soci::sqlite3, pathToTestDatabase);
    confu_soci::createTableForStruct<Board> (sql, { { "gameId", "Game", "id" } });
    confu_soci::createTableForStruct<Game> (sql);
    REQUIRE (doesTableExist<Board> (sql));
    REQUIRE (doesTableExist<Game> (sql));
    WHEN ("record gets inserted in table with foreign key constraints enabled and violated foreign key constraints")
    {
      insertStruct (sql, Game{ "game1" }, true);
      try
        {
          insertStruct (sql, Board{ "board1", "value not in database which violets foreign key constraints" }, true);
          FAIL ("should throw exception but did not throw exception");
        }
      catch (soci::soci_error const &e)
        {
          THEN ("throws exception") { REQUIRE (soci::soci_error::error_category::constraint_violation == e.get_error_category ()); }
        }
    }
  }
  GIVEN ("a connection to a database where the tables exists and foreign key constraints are enabled")
  {
    resetTestDatabase ();
    soci::session sql (soci::sqlite3, pathToTestDatabase);
    confu_soci::createTableForStruct<Board> (sql, { { "gameId", "Game", "id" } });
    confu_soci::createTableForStruct<Game> (sql);
    REQUIRE (doesTableExist<Board> (sql));
    REQUIRE (doesTableExist<Game> (sql));
    sql << "PRAGMA foreign_keys = ON;";
    auto foreignKeysEnabled = 0;
    sql << "PRAGMA foreign_keys;", soci::into (foreignKeysEnabled);
    REQUIRE (foreignKeysEnabled != 0);
    WHEN ("record gets inserted in table with foreign key constraints enabled")
    {
      insertStruct (sql, Game{ "game1" }, true);
      insertStruct (sql, Board{ "board1", "game1" }, true);
      THEN ("foreign keys enabled are still true")
      {
        sql << "PRAGMA foreign_keys;", soci::into (foreignKeysEnabled);
        REQUIRE (foreignKeysEnabled != 0);
      }
    }
  }
  GIVEN ("a connection to a database where the table does exists")
  {
    resetTestDatabase ();
    soci::session sql (soci::sqlite3, pathToTestDatabase);
    confu_soci::createTableForStruct<Game> (sql);
    REQUIRE (doesTableExist<Game> (sql));
    WHEN ("record gets inserted with id and id generated is set to true")
    {
      auto id = "game1";
      auto generatedId = insertStruct (sql, Game{ id }, true, true);
      THEN ("generated id is different from id in struct") { REQUIRE (id != generatedId); }
    }
  }
  GIVEN ("a connection to a database where the table does exists")
  {
    resetTestDatabase ();
    soci::session sql (soci::sqlite3, pathToTestDatabase);
    confu_soci::createTableForStruct<MyClass> (sql);
    REQUIRE (doesTableExist<MyClass> (sql));
    WHEN ("record gets inserted with id and id generated is set to true")
    {
      auto id = -1;
      auto generatedId = insertStruct (sql, MyClass{ id }, true, true);
      THEN ("generated id is different from id in struct") { REQUIRE (generatedId == 1); }
    }
  }
  GIVEN ("a connection to a database where the table does exists")
  {
    resetTestDatabase ();
    soci::session sql (soci::sqlite3, pathToTestDatabase);
    confu_soci::createTableForStruct<IdAndOptional> (sql);
    REQUIRE (doesTableExist<IdAndOptional> (sql));
    WHEN ("record has a member with optional value")
    {
      insertStruct (sql, IdAndOptional{}, true, true);
      THEN ("generated id is different from id in struct")
      {
        auto idAndOptional = findStruct<IdAndOptional> (sql, "id", 1);
        REQUIRE (idAndOptional.has_value ());
        REQUIRE_FALSE (idAndOptional->optionalText.has_value ());
      }
    }
  }
  GIVEN ("a connection to a database where the table does exists")
  {
    resetTestDatabase ();
    soci::session sql (soci::sqlite3, pathToTestDatabase);
    confu_soci::createTableForStruct<MyVector> (sql);
    REQUIRE (doesTableExist<MyVector> (sql));
    WHEN ("record has a member which is a vector of byte")
    {
      insertStruct (sql, MyVector{ 1, std::vector<uint8_t> (1000000, 129) }, true, true);
      THEN ("generated id is different from id in struct")
      {
        auto myVec = findStruct<MyVector> (sql, "id", 1);
        REQUIRE (myVec.has_value ());
        REQUIRE (myVec->someVector.size () == 1000000);
        REQUIRE (myVec->someVector.at (0) == 129);
      }
    }
  }
  GIVEN ("a connection to a database where the table does exists")
  {
    resetTestDatabase ();
    soci::session sql (soci::sqlite3, pathToTestDatabase);
    confu_soci::createTableForStruct<MyVectorStringId> (sql);
    REQUIRE (doesTableExist<MyVectorStringId> (sql));
    WHEN ("record has a member which is a vector of byte")
    {
      insertStruct (sql, MyVectorStringId{ "1", std::vector<uint8_t> (1000000, 129) });
      THEN ("generated id is different from id in struct")
      {
        auto myVec = findStruct<MyVectorStringId> (sql, "id", "1");
        REQUIRE (myVec.has_value ());
        REQUIRE (myVec->someVector.size () == 1000000);
        REQUIRE (myVec->someVector.at (0) == 129);
      }
    }
  }
  GIVEN ("a connection to a database where the table does exists")
  {
    resetTestDatabase ();
    soci::session sql (soci::sqlite3, pathToTestDatabase);
    confu_soci::createTableForStruct<MyVectorStringIdAndEnum> (sql);
    REQUIRE (doesTableExist<MyVectorStringIdAndEnum> (sql));
    WHEN ("record has a vector of byte and enum")
    {
      insertStruct (sql, MyVectorStringIdAndEnum{ "1", std::vector<uint8_t> (1000000, 129), Direction::Down });
      THEN ("generated id is different from id in struct")
      {
        auto myVec = findStruct<MyVectorStringIdAndEnum> (sql, "id", "1");
        REQUIRE (myVec.has_value ());
        REQUIRE (myVec->someVector.size () == 1000000);
        REQUIRE (myVec->someVector.at (0) == 129);
        REQUIRE (myVec->direction == Direction::Down);
      }
    }
  }
}

SCENARIO ("update struct in database with updateStruct", "[updateStruct]")
{
  GIVEN ("a connection to a database where the table exist and has a record")
  {
    resetTestDatabase ();
    soci::session sql (soci::sqlite3, pathToTestDatabase);
    createTableForStruct<EasyClass> (sql);
    auto id = "222";
    insertStruct (sql, EasyClass{ id, 42 });
    auto oldValue = findStruct<EasyClass> (sql, "playerId", "222")->points;
    REQUIRE (doesTableExist<EasyClass> (sql));
    WHEN ("update record")
    {
      double newValue = 1337;
      REQUIRE (updateStruct (sql, EasyClass{ id, newValue }) == id);
      THEN ("the record gets updated in the table")
      {
        REQUIRE (oldValue != newValue);
        REQUIRE (findStruct<EasyClass> (sql, "playerId", std::string{ id })->points == newValue);
      }
    }
  }
  GIVEN ("a connection to a database where the table exists and has no record")
  {
    resetTestDatabase ();
    soci::session sql (soci::sqlite3, pathToTestDatabase);
    createTableForStruct<EasyClass> (sql);
    REQUIRE (doesTableExist<EasyClass> (sql));
    WHEN ("record gets updated")
    {
      try
        {
          updateStruct (sql, EasyClass{ "222", 42 });
          FAIL ("should throw exception but did not throw exception");
        }
      catch (soci::soci_error const &e)
        {
          THEN ("throws exception") { REQUIRE (soci::soci_error::error_category::unknown == e.get_error_category ()); }
        }
    }
  }
}

SCENARIO ("upsert struct in database with upsertStruct", "[upsertStruct]")
{
  GIVEN ("a connection to a database where the table exist and has no record")
  {
    resetTestDatabase ();
    soci::session sql (soci::sqlite3, pathToTestDatabase);
    createTableForStruct<EasyClass> (sql);
    REQUIRE (doesTableExist<EasyClass> (sql));
    REQUIRE (not findStruct<EasyClass> (sql, "playerId", "222").has_value ());
    WHEN ("upsert is called")
    {
      auto easyClassId = upsertStruct (sql, EasyClass{ "222", 42 });
      REQUIRE (easyClassId == "222");
      THEN ("the record gets inserted in the table") { REQUIRE (findStruct<EasyClass> (sql, "playerId", "222").has_value ()); }
    }
  }
  GIVEN ("a connection to a database where the table exist and has a record")
  {
    resetTestDatabase ();
    soci::session sql (soci::sqlite3, pathToTestDatabase);
    createTableForStruct<EasyClass> (sql);
    REQUIRE (doesTableExist<EasyClass> (sql));
    insertStruct (sql, EasyClass{ "222", 42 });
    REQUIRE (findStruct<EasyClass> (sql, "playerId", "222").has_value ());
    double oldValue = findStruct<EasyClass> (sql, "playerId", "222")->points;
    WHEN ("upsert is called")
    {
      double newValue = 1337;
      REQUIRE (oldValue != newValue);
      REQUIRE_NOTHROW (upsertStruct (sql, EasyClass{ "222", newValue }));
      THEN ("the record gets updated")
      {
        REQUIRE (findStruct<EasyClass> (sql, "playerId", "222").has_value ());
        REQUIRE (findStruct<EasyClass> (sql, "playerId", "222")->points != oldValue);
      }
    }
  }
  GIVEN ("a connection to a database where the table does NOT exist")
  {
    resetTestDatabase ();
    soci::session sql (soci::sqlite3, pathToTestDatabase);
    REQUIRE (not doesTableExist<EasyClass> (sql));
    WHEN ("upsert is called")
    {
      try
        {
          upsertStruct (sql, EasyClass{ "222", 42 });
          FAIL ("should throw exception but did not throw exception");
        }
      catch (soci::soci_error const &e)
        {
          THEN ("throws exception") { REQUIRE (soci::soci_error::error_category::system_error == e.get_error_category ()); }
        }
    }
  }
}

SCENARIO ("find struct in database with findStruct", "[findStruct]")
{
  GIVEN ("a connection to a database where the table exist and has a record")
  {
    resetTestDatabase ();
    soci::session sql (soci::sqlite3, pathToTestDatabase);
    createTableForStruct<EasyClass> (sql);
    REQUIRE (doesTableExist<EasyClass> (sql));
    REQUIRE_NOTHROW (insertStruct (sql, EasyClass{ "222", 42 }));
    WHEN ("findStruct")
    {
      REQUIRE_NOTHROW (findStruct<EasyClass> (sql, "playerId", "222"));
      THEN ("the struct has a value") { REQUIRE (findStruct<EasyClass> (sql, "playerId", "222").has_value ()); }
    }
  }
  GIVEN ("a connection to a database where the table exist and has no record")
  {
    resetTestDatabase ();
    soci::session sql (soci::sqlite3, pathToTestDatabase);
    createTableForStruct<EasyClass> (sql);
    REQUIRE (doesTableExist<EasyClass> (sql));
    WHEN ("findStruct")
    {
      REQUIRE_NOTHROW (findStruct<EasyClass> (sql, "playerId", "222"));
      THEN ("the struct has a value") { REQUIRE (not findStruct<EasyClass> (sql, "playerId", "222").has_value ()); }
    }
  }
  GIVEN ("a connection to a database where the table does NOT exist")
  {
    resetTestDatabase ();
    soci::session sql (soci::sqlite3, pathToTestDatabase);
    REQUIRE (not doesTableExist<EasyClass> (sql));
    WHEN ("findStruct")
    {
      try
        {
          findStruct<EasyClass> (sql, "playerId", "222");
          FAIL ("should throw exception but did not throw exception");
        }
      catch (soci::soci_error const &e)
        {
          THEN ("throws exception") { REQUIRE (soci::soci_error::error_category::system_error == e.get_error_category ()); }
        }
    }
  }
}

SCENARIO ("drop table for struct in database with dropTableForStruct", "[dropTableForStruct]")
{
  GIVEN ("a connection to a database where a table does exist")
  {
    resetTestDatabase ();
    soci::session sql (soci::sqlite3, pathToTestDatabase);
    createTableForStruct<EasyClass> (sql);
    REQUIRE (doesTableExist<EasyClass> (sql));
    WHEN ("dropTableForStruct")
    {
      REQUIRE_NOTHROW (dropTableForStruct<EasyClass> (sql));
      THEN ("table does not exist anymore") { REQUIRE (not doesTableExist<EasyClass> (sql)); }
    }
  }
  GIVEN ("a connection to a database where the table does NOT exist")
  {
    resetTestDatabase ();
    soci::session sql (soci::sqlite3, pathToTestDatabase);
    REQUIRE (not doesTableExist<EasyClass> (sql));
    WHEN ("dropTableForStruct")
    {
      try
        {
          dropTableForStruct<EasyClass> (sql);
          FAIL ("should throw exception but did not throw exception");
        }
      catch (soci::soci_error const &e)
        {
          THEN ("throws exception") { REQUIRE (soci::soci_error::error_category::unknown == e.get_error_category ()); }
        }
    }
  }
}

SCENARIO ("create table for mpl list in database with createTables", "[createTables]")
{
  GIVEN ("a connection to a database where a table does NOT exist")
  {
    resetTestDatabase ();
    soci::session sql (soci::sqlite3, pathToTestDatabase);
    typedef boost::mpl::list<EasyClass> myTables;
    REQUIRE (not doesTableExist<EasyClass> (sql));
    WHEN ("createTables")
    {
      REQUIRE_NOTHROW (createTables<myTables> (sql));
      THEN ("table does exist") { REQUIRE (doesTableExist<EasyClass> (sql)); }
    }
  }
  GIVEN ("a connection to a database where the table does exist")
  {
    resetTestDatabase ();
    soci::session sql (soci::sqlite3, pathToTestDatabase);
    typedef boost::mpl::list<EasyClass> myTables;
    createTableForStruct<EasyClass> (sql);
    REQUIRE (doesTableExist<EasyClass> (sql));
    WHEN ("createTables")
    {
      try
        {
          createTables<myTables> (sql);
          FAIL ("should throw exception but did not throw exception");
        }
      catch (soci::soci_error const &e)
        {
          THEN ("throws exception") { REQUIRE (soci::soci_error::error_category::unknown == e.get_error_category ()); }
        }
    }
  }
}

SCENARIO ("drop tables for mpl list in database with dropTables", "[dropTables]")
{
  GIVEN ("a connection to a database where a table does exist")
  {
    resetTestDatabase ();
    soci::session sql (soci::sqlite3, pathToTestDatabase);
    typedef boost::mpl::list<EasyClass> myTables;
    createTableForStruct<EasyClass> (sql);
    REQUIRE (doesTableExist<EasyClass> (sql));
    WHEN ("dropTables")
    {
      REQUIRE_NOTHROW (dropTables<myTables> (sql));
      THEN ("table does not exist") { REQUIRE (not doesTableExist<EasyClass> (sql)); }
    }
  }
  GIVEN ("a connection to a database where the table does not exist")
  {
    resetTestDatabase ();
    soci::session sql (soci::sqlite3, pathToTestDatabase);
    typedef boost::mpl::list<EasyClass> myTables;
    REQUIRE (not doesTableExist<EasyClass> (sql));
    WHEN ("dropTables")
    {
      auto notDropedTables = dropTables<myTables> (sql);
      THEN ("did not drop table because it did not exist") { REQUIRE (notDropedTables.size () == 1); }
    }
  }
  GIVEN ("a connection to a database where one table exists and the other table not")
  {
    resetTestDatabase ();
    soci::session sql (soci::sqlite3, pathToTestDatabase);
    typedef boost::mpl::list<EasyClass> myTables;
    REQUIRE (not doesTableExist<EasyClass> (sql));
    createTableForStruct<Player> (sql);
    REQUIRE (doesTableExist<Player> (sql));
    WHEN ("dropTables")
    {
      auto notDropedTables = dropTables<myTables> (sql);
      THEN ("did not drop table EasyClass because it did not exist") { REQUIRE (notDropedTables.size () == 1); }
    }
  }
}
bool
operator== (EasyClass const &lhs, EasyClass const &rhs)
{
  return lhs.playerId == rhs.playerId && lhs.points == rhs.points;
}
SCENARIO ("find 3 structs and sort them with findStructsOrderBy", "[findStructsOrderBy]")
{
  GIVEN ("a connection to a database where the table exist and has 3 records")
  {
    resetTestDatabase ();
    soci::session sql (soci::sqlite3, pathToTestDatabase);
    createTableForStruct<EasyClass> (sql);
    REQUIRE (doesTableExist<EasyClass> (sql));
    REQUIRE_NOTHROW (insertStruct (sql, EasyClass{ "222", 13 }));
    REQUIRE_NOTHROW (insertStruct (sql, EasyClass{ "111", 2 }));
    REQUIRE_NOTHROW (insertStruct (sql, EasyClass{ "33", 44 }));
    WHEN ("findStructsOrderBy limit to 3 asc")
    {
      auto results = findStructsOrderBy<EasyClass> (sql, 3, "points", OrderMethod::Ascending);
      auto expectedValues = std::vector<EasyClass>{ { "111", 2 }, { "222", 13 }, { "33", 44 } };
      THEN ("3 records are found ordered asc by points") { REQUIRE (results == expectedValues); }
    }
    WHEN ("findStructsOrderBy limit to 2 asc ")
    {
      auto results = findStructsOrderBy<EasyClass> (sql, 2, "points", OrderMethod::Ascending);
      auto expectedValues = std::vector<EasyClass>{ { "111", 2 }, { "222", 13 } };
      THEN ("2 records are found ordered asc by points") { REQUIRE (results == expectedValues); }
    }
    WHEN ("findStructsOrderBy limit to bigger than items in database asc ")
    {
      auto results = findStructsOrderBy<EasyClass> (sql, 100, "points", OrderMethod::Ascending);
      auto expectedValues = std::vector<EasyClass>{ { "111", 2 }, { "222", 13 }, { "33", 44 } };
      THEN ("3 records are found ordered asc by points") { REQUIRE (results == expectedValues); }
    }
    WHEN ("findStructsOrderBy limit to 3 desc")
    {
      auto results = findStructsOrderBy<EasyClass> (sql, 3, "points", OrderMethod::Descending);
      auto expectedValues = std::vector<EasyClass>{ { "33", 44 }, { "222", 13 }, { "111", 2 } };
      THEN ("3 records are found ordered desc by points") { REQUIRE (results == expectedValues); }
    }
    WHEN ("findStructsOrderBy limit to 2 desc ")
    {
      auto results = findStructsOrderBy<EasyClass> (sql, 2, "points", OrderMethod::Descending);
      auto expectedValues = std::vector<EasyClass>{ { "33", 44 }, { "222", 13 } };
      THEN ("2 records are found ordered desc by points") { REQUIRE (results == expectedValues); }
    }
    WHEN ("findStructsOrderBy limit to bigger than items in database desc ")
    {
      auto results = findStructsOrderBy<EasyClass> (sql, 100, "points", OrderMethod::Descending);
      auto expectedValues = std::vector<EasyClass>{ { "33", 44 }, { "222", 13 }, { "111", 2 } };
      THEN ("3 records are found ordered desc by points") { REQUIRE (results == expectedValues); }
    }
  }
}
SCENARIO ("find 0 structs", "[findStructsOrderBy]")
{
  GIVEN ("a connection to a database where the table exist and has 0 records")
  {
    resetTestDatabase ();
    soci::session sql (soci::sqlite3, pathToTestDatabase);
    createTableForStruct<EasyClass> (sql);
    REQUIRE (doesTableExist<EasyClass> (sql));
    WHEN ("findStructsOrderBy limit to 3 asc")
    {
      auto results = findStructsOrderBy<EasyClass> (sql, 3, "points", OrderMethod::Ascending);
      auto expectedValues = std::vector<EasyClass>{};
      THEN ("0 records are found") { REQUIRE (results == expectedValues); }
    }
  }
}
}