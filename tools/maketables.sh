#!/bin/bash

gendir=src/autogen
rm -f $gendir/tables.h $gendir/intdfa.h $gendir/dbldfa.h
python3 tools/gentbl.py -d > $gendir/tables.h
python3 tools/gendfa.py tools/int.jff "intdfa" "int" > $gendir/intdfa.h
python3 tools/gendfa.py tools/dbl.jff "dbldfa" "dbl" > $gendir/dbldfa.h
echo "wrote files $gendir/tables.h $gendir/intdfa.h $gendir/dbldfa.h"
echo "generation done."

