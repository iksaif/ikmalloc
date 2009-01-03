#!/bin/bash

LD_PRELOAD="./libmy_malloc.so" $@
