#ifndef A63B1AD3_B615_4644_A1AD_B0950411D680
#define A63B1AD3_B615_4644_A1AD_B0950411D680

#include <filesystem>
#include <iostream>
#include <test/constant.hxx>
namespace test
{
void
resetTestDatabase ()
{
  std::filesystem::create_directory (std::filesystem::path{ pathToTestDatabase }.parent_path ());
  std::filesystem::copy_file (pathToTemplateDatabase, pathToTestDatabase, std::filesystem::copy_options::overwrite_existing);
}
}
#endif /* A63B1AD3_B615_4644_A1AD_B0950411D680 */
