#!/usr/bin/env bash
IMAGE="./img/PIA17944_Mcam-SOL538-WB-full~~jpg~~.ppm"
STENCIL="./stencils/gaussian.pgm"
ITER=1

# echo "Filtering " $IMAGE " with " $STENCIL " for " $ITER " iterations."
./filter_image $IMAGE $STENCIL $ITER