#!/bin/bash

docker run --rm -v $(pwd):/9cc -w /9cc compilerbook make test
