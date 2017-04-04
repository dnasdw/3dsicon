#!/bin/bash

cwdir=`pwd`
rootdir=`dirname "$0"`
cd "$rootdir"
rootdir=`pwd`
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:"$rootdir"
cd "$cwdir"
"$rootdir/3dsicon" "$1" "$2"
