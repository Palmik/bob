import pyramid.httpexceptions

import sqlite3
import json

def gomoku_json(request) :
    rid = request.matchdict['rid']

    replay_data_query = "SELECT data FROM game_session WHERE id = ?"
    replay_data_row = request.db.execute(replay_data_query, [rid]).fetchone()
    

    if (replay_data_row != None) :
        return json.loads(replay_data_row[0])
    else :
        return pyramid.httpexceptions.HTTPNotFound('Such replay does not exist.')

def gomoku(request) :
    rid = request.matchdict['rid']
    
    player_name_query = """
    SELECT user.name
    FROM   game_session_participant 
       JOIN user 
           ON user.id = game_session_participant.user_id 
    WHERE game_session_participant.game_session_id = ? AND
          game_session_participant.type = ?
    """
    
    player_black_name_row = request.db.execute(player_name_query, [rid, 0]).fetchone();
    player_white_name_row = request.db.execute(player_name_query, [rid, 1]).fetchone();

    if (player_black_name_row != None and player_white_name_row != None) :
        return { "player_black_name" : player_black_name_row[0]
               , "player_white_name" : player_white_name_row[0]
               , "replay_json_url" : request.route_url('gomoku_replay_json', rid=rid)
               }
    else :
        return pyramid.httpexceptions.HTTPNotFound('Such replay does not exist.')

