#ifndef BOB_DATABASE_DATABASE_HPP
#define BOB_DATABASE_DATABASE_HPP

#include <stdexcept>
#include <sqlite3.h>

typedef unsigned int uint;

namespace bob { namespace database
{

enum class game_type : int
{
    gomoku = 1
};

struct game_session_participant_data
{
public:
    game_session_participant_data(int id, int type, int result) : id(id), type(type), result(result) {}
    int id;
    int type;
    int result;
};

class connection
{
public:
    connection(std::string const& file_name);
    ~connection();

    uint authenticate_user(std::string const& user_name, bool& authenticated);
    
    template<typename I>
    uint insert_game_session(game_type game, I pb, I pe, std::string const& replay_data);

private:
    connection(connection const& o);
    
    void create_tables();

    sqlite3* db_connection_m;
};

template <typename I>
uint connection::insert_game_session
    ( game_type game
    , I participant_data_begin
    , I participant_data_end
    , std::string const& replay_data
    )
{
    sqlite3_mutex_enter(sqlite3_db_mutex(db_connection_m));
    sqlite3_exec(db_connection_m, "BEGIN;", 0, 0, 0);
   
    int error = SQLITE_OK;
    sqlite3_stmt* prepared_stmt;
    
    // Insert.
    error = sqlite3_prepare_v2(db_connection_m, "INSERT INTO game_session (game_id, data) VALUES (?, ?);", -1, &prepared_stmt, 0); 
    if (error != SQLITE_OK)
    {
        throw std::runtime_error(std::string("Could not prepare a statement: ") + sqlite3_errmsg(db_connection_m));
    }
    sqlite3_bind_int(prepared_stmt, 1, static_cast<int>(game));
    sqlite3_bind_text(prepared_stmt, 2, replay_data.data(), -1, SQLITE_TRANSIENT);

    if (sqlite3_step(prepared_stmt) != SQLITE_DONE)
    {
        throw std::runtime_error(std::string("Could not execute a statement: ") + sqlite3_errmsg(db_connection_m));
    }
    sqlite3_finalize(prepared_stmt);
 
    // Select.
    error = sqlite3_prepare_v2(db_connection_m, "SELECT last_insert_rowid();", -1, &prepared_stmt, 0);
    if (error != SQLITE_OK)
    {
        throw std::runtime_error(std::string("Could not prepare a statement: ") + sqlite3_errmsg(db_connection_m));
    }
    
    uint result = 0;
    if (sqlite3_step(prepared_stmt) != SQLITE_ROW)
    {
        throw std::runtime_error(std::string("Could not execute a statement: ") + sqlite3_errmsg(db_connection_m));
    }
    else
    {
        result = static_cast<uint>(sqlite3_column_int(prepared_stmt, 0));
    }
    sqlite3_finalize(prepared_stmt);

    // Insert participants.
    char const* insert_participant =
        "INSERT INTO game_session_participant "
                         " ( game_session_id "
                         " , user_id "
                         " , type "
                         " , result "
                         " ) "
                         " VALUES (?, ?, ?, ?);";

    error = sqlite3_prepare_v2(db_connection_m, insert_participant, -1, &prepared_stmt, 0); 
    if (error != SQLITE_OK)
    {
        throw std::runtime_error(std::string("Could not prepare a statement: ") + sqlite3_errmsg(db_connection_m));
    }
    sqlite3_bind_int(prepared_stmt, 1, static_cast<int>(result));

    for (I it = participant_data_begin; it != participant_data_end; ++it)
    {
        sqlite3_bind_int(prepared_stmt, 2, static_cast<int>(it->id)); // user_id
        sqlite3_bind_int(prepared_stmt, 3, static_cast<int>(it->type)); // type
        sqlite3_bind_int(prepared_stmt, 4, static_cast<int>(it->result)); // result

        if (sqlite3_step(prepared_stmt) != SQLITE_DONE)
        {
            throw std::runtime_error(std::string("Could not execute a statement: ") + sqlite3_errmsg(db_connection_m));
        }
        sqlite3_reset(prepared_stmt);
    }
    sqlite3_finalize(prepared_stmt);

    sqlite3_exec(db_connection_m, "COMMIT;", 0, 0, 0);
    sqlite3_mutex_leave(sqlite3_db_mutex(db_connection_m));

    return result;
}

} // namespace database
} // namespace bob

#endif // DATABASE_DATABASE_HPP
