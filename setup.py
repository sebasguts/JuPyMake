from distutils.core import setup, Extension
import sys
import subprocess

if sys.version_info < (3,5):
    macro_list = [ ( "PYTHON_VERSION_OLDER_THREE_FIVE", "1" ) ]
else:
    macro_list = [ ]

if sys.version_info < (3,0):
    FileNotFoundError = OSError

polymake_cflags = [ subprocess.check_output( [ "polymake-config", "--cflags" ] ).strip().decode( 'utf-8' ) ]
polymake_cflags += [ subprocess.check_output( [ "polymake-config", "--includes" ] ).strip().decode( 'utf-8' ) ]
polymake_ldflags = [ subprocess.check_output( [ "polymake-config", "--ldflags" ] ).strip().decode( 'utf-8' ) ]
polymake_ldflags += [ "-lpolymake" ]

setup(
    name = 'JuPyMake',
    version = '0.3',
    description = 'A simple interface to Polymake',
    author = 'Sebastian Gutsche',
    author_email = 'sebastian.gutsche@gmail.com',
    url = 'https://github.com/sebasguts/JuPyMake',
    ext_modules= [ Extension( "JuPyMake",
                              [ "JuPyMake.cpp" ],
                              extra_compile_args=polymake_cflags,
                              extra_link_args=polymake_ldflags,
                              define_macros = macro_list ) ],
    package_data = { '': [ "COPYING", "GPLv2" ] },
)
