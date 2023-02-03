#!/bin/bash

function work()
{
    rm -rf install
    cd code
    make
    make install
    cp -r racebench ../install
    make clean
}

mapfile -t targets < list.txt

for name in "${targets[@]}"; do
    echo "work on $name"
    pushd $name > /dev/null
    work $name
    popd > /dev/null
done
