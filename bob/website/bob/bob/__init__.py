import os
import sqlite3

from wsgiref.simple_server import make_server

from pyramid.config   import Configurator
from pyramid.response import Response
from pyramid.events   import NewRequest

import bob.handlers.replay

def main(global_config, **settings):
    here = os.getcwd()

    # SETTINGS
    settings = {}
    settings['db'] = os.path.join(here, '../../../database.sqlite')
    print(here)

    config = Configurator(settings=settings)
    config.add_static_view('static', 'static', cache_max_age=3600)
    # ROUTES
    config.add_route('replay_json1', '/replay/{rid}.json')
    config.add_route('replay', '/replay/{rid}')
    config.add_route('replay_json2', '/replay/{rid}', accept="application/json")
    # VIEWS
    config.add_view(bob.handlers.replay.handler_json, route_name='replay_json1', renderer='json')
    config.add_view(bob.handlers.replay.handler_json, route_name='replay_json2', renderer='json')
    config.add_view(bob.handlers.replay.handler, route_name='replay', renderer='templates/replay.pt') 

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
