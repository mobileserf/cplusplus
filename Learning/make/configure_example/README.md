#configure.ac modified from apache https://github.com/apache/trafficserver/configure.ac

create configure.ac
#add version file, this is used by autoconf to take lib version
echo "0.0.1" > VERSION

mkdir -p tools/m4

#copy some m4 files to tools/m4. added TS_ to MSERF_
 common.m4
 ax_perl_modules.m4
 ax_cxx_compile_stdcxx.m4
 ax_check_openssl.m4

 #additional modules
 ax_with_curses.m4
 ax_require_defined.m4
 ax_path_generic.m4
 ax_lib_readline.m4
 ax_lib_curl.m4 atomic.m4
 ax_compiler_vendor.m4
 crypto.m4
 zlib.m4
 jansson.m4
 jemalloc.m4
 tcmalloc.m4
 hiredis.m4
 network.m4
 lzma.m4

#create makefile and sub directories
touch Makefile.am
echo "ACLOCAL_AMFLAGS = -I tools/m4" >> Makefile.am
echo "AUTOMAKE_OPTIONS = foreign" >> Makefile.am
echo "SUBDIRS = net" >> Makefile.am # this is subdirectory

#subdirectories and files
mkdir net
#cpp file
touch net/main.cpp
#add follwing content to main.cpp
int main() {
  return 0;
}
#create test.conf.in, configure will create test.conf file out of this, see section 6 in config
touch net/test.conf.in

#create subdirectory makefile
touch net/Makefile.am
#add following content
AUTOMAKE_OPTIONS = foreign
bin_PROGRAMS = mainbin
mainbin_SOURCES = main.cpp

#run autoconf
autoreconf -fi
#runc configure
./configure
#execute make
make


following files/notes created by autoreconf -fi
libtoolize: putting auxiliary files in AC_CONFIG_AUX_DIR, `tools/_aux'.
libtoolize: copying file `tools/_aux/ltmain.sh'
libtoolize: putting macros in AC_CONFIG_MACRO_DIR, `tools/m4'.
libtoolize: copying file `tools/m4/libtool.m4'
libtoolize: copying file `tools/m4/ltoptions.m4'
libtoolize: copying file `tools/m4/ltsugar.m4'
libtoolize: copying file `tools/m4/ltversion.m4'
libtoolize: copying file `tools/m4/lt~obsolete.m4'
configure.ac:458: installing `tools/_aux/compile'
configure.ac:98: installing `tools/_aux/config.guess'
configure.ac:98: installing `tools/_aux/config.sub'
configure.ac:43: installing `tools/_aux/install-sh'
configure.ac:43: installing `tools/_aux/missing'
net/Makefile.am: installing `tools/_aux/depcomp'

rm -rf net/.deps build/  tools/_*  autom4te.cache/ aclocal.m4  configure config.h.in config.h.in~  config.log  Makefile net/Makefile.in config.h  config.nice config.status  libtool Makefile.in  stamp-h1 net/Makefile net/helloworld net/main.o net/test.conf tools/m4/libtool.m4 tools/m4/ltoptions.m4 tools/_aux/ltmain.sh tools/m4/ltsugar.m4 tools/m4/ltversion.m4 tools/m4/lt~obsolete.m4;

autoreconf -fi

./configure
make
