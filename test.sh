#!/usr/bin/env bash
IMAGE="./img/hummingbird~~jpg~~.ppm"
STENCIL="./stencils/gaussian.pgm"
ITER=2

# echo "Filtering " $IMAGE " with " $STENCIL " for " $ITER " iterations."
./filter_image $IMAGE $STENCIL $ITER