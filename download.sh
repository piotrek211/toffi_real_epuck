#/bin/env bash -e

USERNAME=`whoami`

# Installation directory (specify a full path!)
INSTALL_DIR=/home/${USERNAME}/Desktop/toffi_real_epuck/argos3-dist

# ARGoS3 repository
ARGOS3_REPOSITORY=git://github.com/ilpincy/argos3.git
ARGOS3_BRANCH=master
ARGOS3_TAG=3.0.0-beta48
ARGOS3_DIR=argos3

# IRIDIA e-puck repository
EPUCK_REPOSITORY=git://github.com/demiurge-project/argos3-epuck.git
EPUCK_BRANCH=master
EPUCK_TAG=v48
EPUCK_DIR=argos3-epuck


# IRIDIA e-puck dao repository
EPUCK_DAO_REPOSITORY=git://github.com/piotrek211/epuck-dao.git
EPUCK_DAO_BRANCH=master
EPUCK_DAO_DIR=demiurge-epuck-dao

# IRIDIA arena repository
ARENA_REPOSITORY=git://github.com/demiurge-project/argos3-arena.git
ARENA_BRANCH=master
ARENA_DIR=argos3-arena


function error() {
    echo
    echo "Error, script halted."
    exit 1
}

# Fetches the latest sources from a git repository
# $1: full repository URL
# $2: repository branch
# $3: output directory
function get_code_with_tag() {
    local REPO=$1
    local BRANCH=$2
    local TAG=$3
    local OUTDIR=$4
  
    if [ ! -d $OUTDIR ]; then
        echo -n "Downloading... "
        git clone --branch=$BRANCH $REPO $OUTDIR || error
        git checkout tags/$TAG|| error
    else
        echo -n "Updating... "
        cd $OUTDIR || error
        git checkout tags/$TAG|| error
        cd ..
    fi
    echo "done"
}

function get_code() {
    local REPO=$1
    local BRANCH=$2
    local OUTDIR=$3
  
    if [ ! -d $OUTDIR ]; then
        echo -n "Downloading... "
        git clone --branch=$BRANCH $REPO $OUTDIR || error
    else
        echo -n "Updating... "
        cd $OUTDIR || error
        git checkout master|| error
        cd ..
    fi
    echo "done"
}




# Builds and installs ARGoS3
function download_argos3() {
    echo "+++ Downloading ARGoS3 +++"
    get_code_with_tag $ARGOS3_REPOSITORY $ARGOS3_BRANCH $ARGOS3_TAG $ARGOS3_DIR
}

# Builds and installs e-puck
function download_epuck() {
    echo "+++ Downloading e-puck +++"
    get_code_with_tag $EPUCK_REPOSITORY $EPUCK_BRANCH $EPUCK_TAG $EPUCK_DIR
}

function download_epuck_dao() {
    echo "+++ Downloading e-puck dao +++"
    get_code $EPUCK_DAO_REPOSITORY $EPUCK_DAO_BRANCH $EPUCK_DAO_DIR
}

function download_arena() {
    echo "+++ Downloading arena +++"
    get_code $ARENA_REPOSITORY $ARENA_BRANCH $ARENA_DIR
}


# Go through packages
download_argos3
download_epuck
download_epuck_dao
#download_arena
