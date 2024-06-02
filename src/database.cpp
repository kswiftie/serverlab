#include "..\\include\\database.h"

database::database() {
    this->host = "127.0.0.1";
    this->port = "5432";
    this->user = "postgres";
    this->password = "postgres";
}

database::database(std::string user, std::string password, std::string host, std::string port) {
    this->host = host;
    this->port = port;
    this->user = user;
    this->password = password;
}

void database::create_test_db() {
    std::string request = "psql -c \"CREATE DATABASE musicdb;\" \"user=" + this->user +
    " password=" + this->password +
    " host=" + this->host +
    " port=" + this->port + "\" ";
    system(request.c_str());
    std::string requests = "psql -c \"";
    std::ifstream file("create_db.txt");
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            requests += line + "\n";
        }
        file.close();
    }
    requests.pop_back();
    requests += "\" \"dbname=musicdb user=" + this->user +
    " password=" + this->password +
    " host=" + this->host +
    " port=" + this->port + "\"";
    system(requests.c_str());
}

void database::fill_test_db() {
    std::random_device rd;
    int genres = 4, songs = 150, albums = 20, artists = 15;

    std::string request = "\
psql -c \"INSERT INTO genre (name) \
VALUES " + fill_test_db_main_table(genres, "'genre") + " " +

"INSERT INTO song (name) \
VALUES " + fill_test_db_main_table(songs, "'song") + " " +

"INSERT INTO album (name) \
VALUES " + fill_test_db_main_table(albums, "'album") + " " +

"INSERT INTO artist (name) \
VALUES " + fill_test_db_main_table(artists, "'artist") + " "

+ "INSERT INTO album_song (song_id, album_id) \
VALUES " + fill_test_db_side_table(albums - 1, songs, "album", "song", rd) + " " +

"INSERT INTO artist_song (song_id, artist_id) \
VALUES " + fill_test_db_side_table(artists - 1, songs, "artist", "song", rd) + " "

"INSERT INTO genre_song (song_id, genre_id) \
VALUES " + fill_test_db_side_table(genres - 1, songs, "genre", "song", rd);
    request += "\" \"dbname=musicdb user=" + this->user +
    " password=" + this->password +
    " host=" + this->host +
    " port=" + this->port + "\"";
    system(request.c_str());
}

std::string database::fill_test_db_main_table(int n, std::string table) {
    std::string res = "";
    for (int i = 1; i <= n; ++i) {
        res += "(";
        res += table;
        res += std::to_string(i) + (std::string)"'";
        res += ")";
        if (i < n) res += ", ";
        else res += "; ";
    }
    return res;
}

std::string database::fill_test_db_side_table(int to_connect_size, int connect_size, std::string to_connect, std::string connect, std::random_device& rd) {
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> get_to_connect_id(1, to_connect_size);
    std::string res = "";
    // to_connect (artists, albums, genres) in this case less than connect (songs)
    for (int i = 1; i <= to_connect_size; ++i) {
        std::string cur_num = std::to_string(i);
        res += "(";
        res += cur_num;
        res += ", ";
        res += cur_num;
        res += ")";
        res += ", ";
    }
    for (int i = to_connect_size + 1; i <= connect_size; ++i) {
        res += "(";
        int tmp = get_to_connect_id(gen);
        res += std::to_string(i);
        res += ", ";
        res += std::to_string(tmp);
        res += ")";
        if (i < connect_size) res += ", ";
        else res += "; ";
    }
    return res;
}

void database::add_entry(std::string table, std::string columns, std::string values) {
    std::string request = "psql -c \"INSERT INTO " + table + columns + " VALUES " + values +
    " ;\" \"dbname=musicdb user=" + this->user +
    " password=" + this->password +
    " host=" + this->host +
    " port=" + this->port + "\"";
    system(request.c_str());
}

void database::get_info_from_table(std::string table, std::string columns) {
    std::string request = "psql -c \"SELECT CONCAT" + columns + " FROM " + table +
    ";\" \"dbname=musicdb user=" + this->user +
    " password=" + this->password +
    " host=" + this->host +
    " port=" + this->port + "\" > tmp.txt";
    system(request.c_str());
}

void database::hard_request(std::string request) {
    std::string res_request = "psql -c \"" + request + 
    " \" \"dbname=musicdb user=" + this->user +
    " password=" + this->password +
    " host=" + this->host +
    " port=" + this->port + "\" > tmp.txt";
    system(res_request.c_str());
}