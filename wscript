from waflib.Configure import conf
from waflib.Tools     import waf_unit_test

def options(opt) :
    opt.load('compiler_cxx boost waf_unit_test')

def configure(cnf) :
    cnf.load('compiler_cxx boost waf_unit_test')
    
    flags = ['-static', '-Wl,-Bstatic', '-Wall', '-Wextra', '-std=c++0x', '-ggdb3']
    cnf.check_cxx(cxxflags = flags)
    cnf.env.append_value('CXXFLAGS', flags)

def build(bld) :
    bld.add_post_fun(waf_unit_test.summary)

    bld.recurse('bob/database')
    bld.recurse('bob/client')
    bld.recurse('bob/gomoku')

@conf
def create_boost_test(bld, uses, libs, parent_name, name) :
    bld( features = 'cxx cxxprogram test'
       , source   = name + '.cpp'
       , target   = parent_name + '/tests/' + name
       , use      = uses
       , lib      = libs + ['boost_unit_test_framework']
       )

