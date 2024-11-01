#!/bin/bash

rm -f src/tables.h src/intdfa.h src/dbldfa.h
python3 tools/gentbl.py -d > src/tables.h
python3 tools/gendfa.py tools/int.jff "intdfa" "int" > src/intdfa.h
python3 tools/gendfa.py tools/dbl.jff "dbldfa" "dbl" > src/dbldfa.h
echo "wrote files src/tables.h src/intdfa.h src/dbldfa.h"
echo "generation done."

