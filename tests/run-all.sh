#!/bin/sh
# Run all tests
set -e

for T in $(find "$(dirname "$0")" -name test.sh -type f -executable); do
	timeout -k 120 60 "$T"
done
