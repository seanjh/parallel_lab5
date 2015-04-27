#!/usr/bin/env bash
IMAGE="./img/doge.jpg"
STENCIL="./stencils/gaussian.pgm"
ITER=1

echo "Filtering " $IMAGE " with " $STENCIL " for " $ITER " iterations."
./filter_image $IMAGE $STENCIL $ITER