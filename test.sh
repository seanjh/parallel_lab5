#!/usr/bin/env bash
IMG_PATH="./img/"
IMG_FILE="doge.jpg"
STENCIL_PATH="./stencils/"
STENCIL_FILE="edge.pgm"
ITER=1

echo "Filtering " $IMG_FILE " with " $STENCIL_FILE " for " $ITER " iterations."
./filter_image $  $IMG_PATH$IMG_FILE $STENCIL_PATH$STENCIL_FILE $ITER