#ifndef FE334B5B_FA67_454D_A6F5_A1CBF7D02BB7
#define FE334B5B_FA67_454D_A6F5_A1CBF7D02BB7
#include <filesystem>
#include <string>
namespace test
{

std::string const pathToTemplateDatabase = std::filesystem::path{ PATH_TO_BINARY }.parent_path ().parent_path ().append ("test_data").append ("test_db");
std::string const pathToTestDatabase = std::filesystem::path{ PATH_TO_BINARY }.append ("test_data").append ("test_db");
}

#endif /* FE334B5B_FA67_454D_A6F5_A1CBF7D02BB7 */
