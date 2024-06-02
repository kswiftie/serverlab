#include <cstdlib>
#include <string>
#include <iostream>
#include <fstream>
#include <random>

class database {
private:
    std::string user;
    std::string password;
    std::string host;
    std::string port;
public:
    database();
    database(std::string user, std::string password, std::string host, std::string port);
    void create_database();
    void create_test_db();
    void fill_test_db();
    std::string fill_test_db_main_table(int n, std::string table);
    std::string fill_test_db_side_table(int to_connect_size, int connect_size, std::string to_connect, std::string connect, std::random_device& rd);
    void add_entry(std::string table, std::string columns, std::string values);
    void get_info_from_table(std::string table, std::string columns);
    void hard_request(std::string request);
};