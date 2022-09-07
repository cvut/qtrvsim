#!/bin/bash
TESTER=$1
BIN=$2
EXTERNAL=$3
NULL=/dev/null

# Test without required cli bin
python $TESTER 1> $NULL 2> $NULL
RES1=$?

# Test default
python $TESTER $BIN 1> $NULL 2> $NULL
TMP=$?
if [ $TMP -eq 0 ];
then
    echo "default   passed"
else
    echo "default   failed"
fi

# Test help
python $TESTER $BIN 1> $NULL 2> $NULL -h
TMP=$?
if [ $TMP -eq 0 ];
then
    echo "help      passed"
else
    echo "help      failed"
fi

# Test no-32
python $TESTER $BIN 1> $NULL 2> $NULL --no-32
TMP=$?
if [ $TMP -eq 0 ];
then
    echo "no-32     passed"
else
    echo "no-32     failed"
fi

# Test no-64
python $TESTER $BIN 1> $NULL 2> $NULL --no-64
TMP=$?
if [ $TMP -eq 0 ];
then
    echo "no-64     passed"
else
    echo "no-64     failed"
fi

# Test external
python $TESTER $BIN 1> $NULL 2> $NULL -E $EXTERNAL
TMP=$?
if [ $TMP -eq 0 ];
then
    echo "external  passed"
else
    echo "external  failed"
fi

# Test multiply
python $TESTER $BIN 1> $NULL 2> $NULL -M
TMP=$?
if [ $TMP -eq 0 ];
then
    echo "multiply  passed"
else
    echo "multiply  failed"
fi

# Test atomic
python $TESTER $BIN 1> $NULL 2> $NULL -A
TMP=$?
if [ $TMP -eq 0 ];
then
    echo "atomic    passed"
else
    echo "atomic    failed"
fi

# Test CSR
python $TESTER $BIN 1> $NULL 2> $NULL --CSR
TMP=$?
if [ $TMP -eq 0 ];
then
    echo "CSR       passed"
else
    echo "CSR       failed"
fi

# Test pipeline
python $TESTER $BIN 1> $NULL 2> $NULL --pipeline
TMP=$?
if [ $TMP -eq 0 ];
then
    echo "pipeline  passed"
else
    echo "pipeline  failed"
fi

# Test cache
python $TESTER $BIN 1> $NULL 2> $NULL --cache
TMP=$?
if [ $TMP -eq 0 ];
then
    echo "cache     passed"
else
    echo "cache     failed"
fi

# Test no-pass
python $TESTER $BIN 1> $NULL 2> $NULL --no-pass
TMP=$?
if [ $TMP -eq 0 ];
then
    echo "no-pass   passed"
else
    echo "no-pass   failed"
fi

# Test no-dump
python $TESTER $BIN 1> $NULL 2> $NULL --no-dump
TMP=$?
if [ $TMP -eq 0 ];
then
    echo "no-dump   passed"
else
    echo "no-dump   failed"
fi

# Test d-regs
python $TESTER $BIN 1> $NULL 2> $NULL --d-regs
TMP=$?
if [ $TMP -eq 0 ];
then
    echo "d-regs    passed"
else
    echo "d-regs    failed"
fi

# Test file
python $TESTER $BIN 1> $NULL 2> $NULL --file
TMP=$?
if [ $TMP -eq 0 ];
then
    echo "file      passed"
else
    echo "file      failed"
fi

# Test rebuild
python $TESTER $BIN 1> $NULL 2> $NULL -R
TMP=$?
if [ $TMP -eq 0 ];
then
    echo "rebuild   passed"
else
    echo "rebuild   failed"
fi

# Test seltest
python $TESTER $BIN 1> $NULL 2> $NULL -S
TMP=$?
if [ $TMP -eq 0 ];
then
    echo "seltest   passed"
else
    echo "seltest   failed"
fi

# Test clean
python $TESTER $BIN 1> $NULL 2> $NULL --clean
TMP=$?
if [ $TMP -eq 0 ];
then
    echo "clean     passed"
else
    echo "clean     failed"
fi