#include <bob/database/database.hpp>

namespace bob { namespace database
{

connection::connection(std::string const& file_name)
    : db_connection_m(0)
{
    int error = SQLITE_OK;
    error = sqlite3_open_v2(file_name.c_str(), &db_connection_m, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, 0);
 
    if (error != SQLITE_OK)
    {
        throw std::runtime_error(std::string("Could not estabilish database connection: ") + sqlite3_errmsg(db_connection_m));
    }

    create_tables();
}

connection::~connection()
{
    sqlite3_close(db_connection_m);
}

uint connection::authenticate_user(std::string const& name, bool& authenticated)
{
    authenticated = true;

    // Insert or ignore.
    sqlite3_mutex_enter(sqlite3_db_mutex(db_connection_m));
    sqlite3_exec(db_connection_m, "BEGIN;", 0, 0, 0);
    
    int error = SQLITE_OK;
    sqlite3_stmt* prepared_stmt;
    
    char const* insert = "INSERT OR IGNORE INTO user (name) VALUES (?);";
    error = sqlite3_prepare_v2(db_connection_m, insert, -1, &prepared_stmt, 0); 
    if (error != SQLITE_OK)
    {
        throw std::runtime_error(std::string("Could not prepare a statement: ") + sqlite3_errmsg(db_connection_m));
    }
    sqlite3_bind_text(prepared_stmt, 1, name.data(), -1, SQLITE_TRANSIENT);

    if (sqlite3_step(prepared_stmt) != SQLITE_DONE)
    {
        throw std::runtime_error(std::string("Could not execute a statement: ") + sqlite3_errmsg(db_connection_m));
    }
    sqlite3_finalize(prepared_stmt);

    sqlite3_exec(db_connection_m, "COMMIT;", 0, 0, 0);
    sqlite3_mutex_leave(sqlite3_db_mutex(db_connection_m));
    
    // Select.
    char const* select = "SELECT (user.id) FROM user WHERE (user.name = ?);";
    error = sqlite3_prepare_v2(db_connection_m, select, -1, &prepared_stmt, 0);
    if (error != SQLITE_OK)
    {
        throw std::runtime_error(std::string("Could not prepare a statement: ") + sqlite3_errmsg(db_connection_m));
    }
    sqlite3_bind_text(prepared_stmt, 1, name.data(), -1, SQLITE_TRANSIENT);
    
    uint result = 0;
    if (sqlite3_step(prepared_stmt) != SQLITE_ROW)
    {
        authenticated = false;
        throw std::runtime_error(std::string("Could not execute a statement: ") + sqlite3_errmsg(db_connection_m));
    }
    else
    {
        result = static_cast<uint>(sqlite3_column_int(prepared_stmt, 0));
    }
    sqlite3_finalize(prepared_stmt);

    return result;
}


void connection::create_tables()
{
    sqlite3_mutex_enter(sqlite3_db_mutex(db_connection_m));
    sqlite3_exec(db_connection_m, "BEGIN;", 0, 0, 0);
    
    char const* game =
       "CREATE TABLE IF NOT EXISTS game "
       "( id   INTEGER PRIMARY KEY "
       ", name TEXT "
       ", UNIQUE(name) "
       ");";
    
    char const* game_session =
       "CREATE TABLE IF NOT EXISTS game_session "
       "( id      INTEGER PRIMARY KEY "
       ", game_id INTEGER "
       ", data    TEXT " // Can differ game from game, but it is usually replay data in JSON.
       "); ";
    
    char const* game_session_participant =
       "CREATE TABLE IF NOT EXISTS game_session_participant "
       "( user_id         INTEGER "
       ", game_session_id INTEGER "
       ", result          INTEGER " // Can differ game from game, but it is usually one of (defeat, draw, victory). 
       ", type            INTEGER " // Can differ game from game, for connect this is either (0 == white) or (1 == black).
       ");";
    
    char const* user =
       "CREATE TABLE IF NOT EXISTS user "
       "( id   INTEGER PRIMARY KEY "
       ", name TEXT "
       ", UNIQUE(name) "
       ");";

    sqlite3_exec(db_connection_m, game, 0, 0, 0);
    sqlite3_exec(db_connection_m, user, 0, 0, 0);
    sqlite3_exec(db_connection_m, game_session, 0, 0, 0);
    sqlite3_exec(db_connection_m, game_session_participant, 0, 0, 0);

    sqlite3_exec(db_connection_m, "COMMIT;", 0, 0, 0);
    sqlite3_mutex_leave(sqlite3_db_mutex(db_connection_m));
}

} // namespace database
} // namespace bob

