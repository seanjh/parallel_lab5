#!/usr/bin/env bash
NETPBM_COMMAND="jpegtopnm"
IMAGE="./img/skater.jpg"
PPM_IMAGE="./img/skater.ppm"
PGM_STENCIL="./stencils/identity.pgm"
OUTPUT="./img/OUTPUT.ppm"
FINAL_IMAGE="./img/OUTPUT.jpg"
ITERATIONS=1

echo
echo Running NetPBM Original to PPM conversion: $NETPBM_COMMAND $IMAGE
echo "$NETPBM_COMMAND $IMAGE | pnmtopnm -plain > $PPM_IMAGE"
$NETPBM_COMMAND $IMAGE | pnmtopnm -plain > $PPM_IMAGE
echo Finished converting image to PPM

echo
echo Applying image filter to PPM...
echo Source:  $PPM_IMAGE
echo Stencil: $PGM_STENCIL
echo Output:  $OUTPUT

echo
# echo "Filtering " $IMAGE " with " $STENCIL " for " $ITER " iterations."
./filter_image $PPM_IMAGE $PGM_STENCIL $OUTPUT $ITERATIONS
echo Filtered PPM written to $OUTPUT

echo
echo Running NetPBM PPM to Original conversion:
echo "$NETPBM_COMMAND $OUTPUT > $FINAL_IMAGE"
pnmtojpeg $OUTPUT > $FINAL_IMAGE