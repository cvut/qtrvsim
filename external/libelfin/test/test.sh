#!/bin/bash

die() {
    echo "$@"
    exit 1
}

(cd ../examples && make --quiet) || die "failed to build examples"

dumps="sections segments lines syms tree"
binaries=example
compilers="gcc-4.9.2 gcc-6.2.1-s390x"

if [[ $1 == --make-golden ]]; then
    MODE=make-golden
fi

output=$(mktemp --tmpdir libelfin.XXXXXXXXXX)
trap "rm -f $output $output.out" EXIT

FAILED=0
for dump in $dumps; do
    for binary in $binaries; do
        for compiler in $compilers; do
            if [[ $MODE == make-golden ]]; then
                ../examples/dump-$dump golden-$compiler/$binary > golden-$compiler/$dump || \
                    die "failed to create golden output"
                continue
            fi

            PASS=1

            # Save stdout and stderr and redirect output to temporary file.
            exec 3>&1 4>&2 1>$output 2>&1

            # Run the test.
            ../examples/dump-$dump golden-$compiler/$binary >& $output.out
            STATUS=$?
            if [[ $STATUS != 0 ]]; then
                PASS=0
                echo "failed: exit status $STATUS"
            fi
            if ! diff -u golden-$compiler/$dump $output.out; then
                PASS=0
            fi

            # Restore FDs.
            exec 1>&3 2>&4 3>&- 4>&-

            # Report results.
            if [[ $PASS == 0 ]]; then
                FAILED=$((FAILED + 1))
                echo -n "FAIL "
            else
                echo -n "PASS "
            fi
            echo dump-$dump golden-$compiler/$binary

            if [[ $PASS == 0 ]]; then
                sed 's/^/\t/' $output
            fi
        done
    done
done

if [[ $FAILED != 0 ]]; then
    echo "$FAILED test(s) failed"
    exit 1
fi
