import pyramid.httpexceptions

import sqlite3
import json

def handler_json(request) :
    rid = request.matchdict['rid']

    res = request.db.execute("SELECT data FROM game_replay WHERE id = ?", [rid])
    row = res.fetchone()

    if (row != None) :
        return json.loads(row[0])
    else :
        return HTTPNotFound('Such replay does not exist.')

def handler(request) :
    rid = request.matchdict['rid']
    
    return {"replay_json_url" : request.route_url('replay_json1', rid=rid)}
