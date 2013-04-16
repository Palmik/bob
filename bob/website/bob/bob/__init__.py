import os
import sqlite3

from wsgiref.simple_server import make_server

from pyramid.config   import Configurator
from pyramid.response import Response
from pyramid.events   import NewRequest

import bob.handlers.replay
import bob.handlers.static

def main(global_config, **settings):
    here = os.getcwd()

    # SETTINGS
    settings = {}
    settings['db'] = os.path.join(here, '../../../database.sqlite')
    print(here)

    config = Configurator(settings=settings)
    config.add_static_view('static', 'static', cache_max_age=3600)
    # ROUTES
    config.add_route('index', '/')
    
    config.add_route('connect_replay_json', '/g/connect/replay/{rid}.json')
    config.add_route('connect_replay', '/g/connect/replay/{rid}')
    # VIEWS
    config.add_view(bob.handlers.static.static, route_name='index', renderer='templates/index.pt')
    
    config.add_view(bob.handlers.replay.connect_json, route_name='connect_replay_json', renderer='json')
    config.add_view(bob.handlers.replay.connect, route_name='connect_replay', renderer='templates/connect/replay.pt') 

    # EVENT SUBSCRIBERS
    config.add_subscriber(database_connect, NewRequest)

    app = config.make_wsgi_app()
    return app

def database_connect(event) :
    request  = event.request
    settings = request.registry.settings
    request.db = sqlite3.connect(settings['db'])
    request.add_finished_callback(database_disconnect)

def database_disconnect(request) :
    request.db.close()
