#include "convenienceFunctionForSoci.hxx"

namespace confu_soci
{
bool
doesTableExist (soci::session &sql, std::string const &tableName)
{
  try
    {
      soci::rowset<std::string> rs = (sql.prepare << "select current_timestamp from " << tableName << " limit 1;");
      return true;
    }
  catch (...)
    {
    }
  return false;
}

}