#include "util.h"
#include <stdio.h>
#include <stdlib.h>

void errif(bool condition, const char *errmsg){
    if(condition){
        // util.cpp left intentionally minimal; errif is provided as macro in util.h
        #include "util.h"

        // no functions needed for now; header macro is enough
