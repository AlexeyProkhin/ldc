#!/usr/bin/env bash

OLDHOME=$HOME
export HOME=`pwd`

goerror(){
    export HOME=$OLDHOME
    echo "="
    echo "= *** Error ***"
    echo "="
    exit 1
}

make clean -fldc-gcc.mak           || goerror
make lib doc install -fldc-gcc.mak || goerror
make clean -fldc-gcc.mak           || goerror
chmod 644 ../import/*.di           || goerror

export HOME=$OLDHOME