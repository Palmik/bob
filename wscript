from waflib.Configure import conf
from waflib.Tools     import waf_unit_test

def options(opt) :
    opt.load('compiler_cxx boost waf_unit_test')

def configure(cnf) :
    cnf.load('compiler_cxx boost waf_unit_test')
    cnf.check_cxx(cxxflags = ['-std=c++11', '-ggdb3'])
    cnf.env.append_value('CXXFLAGS', ['-std=c++11', '-ggdb3'])

def build(bld) :
    bld.add_post_fun(waf_unit_test.summary)

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

