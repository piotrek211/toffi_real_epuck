#/bin/env bash -e

USERNAME=`whoami`

# Installation directory (specify a full path!)
INSTALL_DIR=/home/${USERNAME}/Desktop/toffi_real_epuck/argos3-dist

ARGOS3_DIR=argos3
EPUCK_DIR=argos3-epuck
EPUCK_DAO_DIR=demiurge-epuck-dao
TOFFI_LOOP_FUNCTIONS_DIR=experiments-loop-functions
ARENA_DIR=argos3-arena


function error() {
    echo
    echo "Error, script halted."
    exit 1
}

# Creates the build directory, builds the code, and installs binaries
# $1: source location
# $*: extra CMake args
function do_build_install() {
    local SRC=../$1
    shift
    rm -rf build || error
    echo -n "Compiling... "
    mkdir build || error
    cd build || error
    cmake -DCMAKE_INSTALL_PREFIX=$INSTALL_DIR \
          -DCMAKE_BUILD_TYPE=Release \
          $* $SRC || error
    make -j4 || error
    echo "done"
    echo "Installing... "
    make install || error
    cd ..
    echo "done"
}

# Creates the build directory and builds the code
# $1: source location
# $*: extra CMake args
function do_build() {
    local SRC=../$1
    shift
    rm -rf build || error
    echo -n "Compiling... "
    mkdir build || error
    cd build || error
    cmake -DCMAKE_BUILD_TYPE=Release \
          $* $SRC || error
    make -j4 || error
    echo "done"
    cd ..
}

# Builds and installs ARGoS3
function do_argos3() {
    echo "+++ ARGoS3 +++"
    cd $ARGOS3_DIR
    do_build_install src -DARGOS_INSTALL_LDSOCONF=OFF -DARGOS_DOCUMENTATION=OFF
    cd ..
    echo -n "Deleting default e-puck... "
    rm -rf \
        $INSTALL_DIR/include/argos3/plugins/robots/e-puck \
        $INSTALL_DIR/lib/argos3/lib*epuck*.so
    echo "done"
}

# Builds and installs e-puck
function do_epuck() {
    echo "+++ e-puck +++"
    export PKG_CONFIG_PATH=$INSTALL_DIR/lib/pkgconfig
    cd $EPUCK_DIR
    do_build_install src
    cd ..
}

function do_epuck_dao() {
    echo "+++ e-puck dao +++"
    cd $EPUCK_DAO_DIR
    do_build_install
    cd ..
}



function do_toffi_loop_functions() {
  echo "+++ toffi loop functions +++"
  cd $TOFFI_LOOP_FUNCTIONS_DIR
  do_build_install
  cd ..
}

function do_arena() {
  echo "+++ arena  +++"
  cd $ARENA_DIR
  do_build_install src
  cd ..
}




# Creates the setup script
function do_create_setup_script() {
    local SCRIPTFILE=$INSTALL_DIR/bin/setup_argos3
    echo "export PKG_CONFIG_PATH=$INSTALL_DIR/lib/pkgconfig" > $SCRIPTFILE
    echo "export ARGOS_PLUGIN_PATH=$INSTALL_DIR/lib/argos3" >> $SCRIPTFILE
    echo "export LD_LIBRARY_PATH=\$ARGOS_PLUGIN_PATH:/usr/java/jdk1.6.0_43/jre/lib/amd64:/usr/java/jdk1.6.0_43/jre/lib/amd64/server:/usr/java/jdk1.6.0_43/jre/lib/amd64/xawt" >> $SCRIPTFILE
    echo "export PATH=$PATH:$INSTALL_DIR/bin" >> $SCRIPTFILE
    echo "Execute the following command to setup the environment for ARGoS3:"
    echo
    echo "  source $SCRIPTFILE"
}

# Erase and re-create installation directory
rm -rf $INSTALL_DIR
mkdir $INSTALL_DIR

# Go through packages
do_argos3
do_epuck
do_epuck_dao
do_arena
do_toffi_loop_functions

# Create the setup script
do_create_setup_script
