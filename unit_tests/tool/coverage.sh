#!/bin/bash

mkdir gcov
cd gcov
rm -f coverage.cc coverage.h

for gdcafile in $( find ../ -name nb_*.gcda );
do
  echo "${gdcafile}" >> coverage.cc
  gcov "${gdcafile}" | grep -v -E "nb_.*test.cc.gcov" | grep -B2 -E "nb_.*\.cc\.gcov" >> coverage.cc
  echo "" >> coverage.cc
  echo "${gdcafile}" >> coverage.h
  gcov "${gdcafile}" | grep -v -E "nb_.*test.cc.gcov" | grep -B2 -E "nb_.*\.h\.gcov" >> coverage.h
  echo "" >> coverage.h
done
