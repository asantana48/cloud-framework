#!/bin/bash
dir=../testbed

function make_old {
    echo "Modified: $1"
    touch -a -t 201611161200.10 $dir/$1
}

make_old 'small_old'
make_old 'medium_old'
make_old 'large_old'