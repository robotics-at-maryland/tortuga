#! /bin/bash

# makeTrainImages.sh
# Robotics At Maryland
# David Love <loved@umd.edu>
# a bash script using ImageMagick to create variations of an image on which to train a network
# arguments: 1 - the image to use as a base, 2 - the extension of the image, 3 - the size to make the images

# some variables we need
IN=$1
EXTENSION=$2
SIZE=$3
BASENAME=$( basename -s $EXTENSION $IN )
OUTDIR=$PWD/$BASENAME
NEWBASE=$OUTDIR/$BASENAME
NEWORIG=$NEWBASE$EXTENSION

# make the new output directory
mkdir $OUTDIR

# make the new original
convert $IN -monochrome -resize $SIZEx$SIZE $NEWORIG

# make several blured images
convert $NEWORIG -blur 2x2 $NEWBASE-blur2$EXTENSION
convert $NEWORIG -blur 3x3 $NEWBASE-blur3$EXTENSION
convert $NEWORIG -blur 4x4 $NEWBASE-blur4$EXTENSION
convert $NEWORIG -blur 5x5 $NEWBASE-blur5$EXTENSION
convert $NEWORIG -blur 6x6 $NEWBASE-blur2$EXTENSION

# change the contrast
convert $NEWORIG -contrast $NEWBASE-constrast1$EXTENSION
convert $NEWORIG +contrast $NEWBASE-constrast2$EXTENSION

# emboss a few images
convert $NEWORIG -emboss 2x2 $NEWBASE-emboss2$EXTENSION
convert $NEWORIG -emboss 3x3 $NEWBASE-emboss3$EXTENSION
convert $NEWORIG -emboss 4x4 $NEWBASE-emboss4$EXTENSION

# digital enhancement and equalization
convert $NEWORIG -enhance $NEWBASE-enhance$EXTENSION
convert $NEWORIG -equalize $NEWBASE-equalize$EXTENSION

# fuzz the image
convert $NEWORIG -fuzz 2 $NEWBASE-fuzz2$EXTENSION
convert $NEWORIG -fuzz 3 $NEWBASE-fuzz3$EXTENSION
convert $NEWORIG -fuzz 4 $NEWBASE-fuzz4$EXTENSION
convert $NEWORIG -fuzz 5 $NEWBASE-fuzz5$EXTENSION
convert $NEWORIG -fuzz 6 $NEWBASE-fuzz6$EXTENSION

# make some imploded and exploded images
convert $NEWORIG -implode 1 $NEWBASE-implode1$EXTENSION
convert $NEWORIG -implode -1 $NEWBASE-implode-1$EXTENSION
convert $NEWORIG -implode -2 $NEWBASE-implode-2$EXTENSION

# use some median filters
convert $NEWORIG -median 1 $NEWBASE-median1$EXTENSION
convert $NEWORIG -median 2 $NEWBASE-median2$EXTENSION
convert $NEWORIG -median 3 $NEWBASE-median3$EXTENSION
convert $NEWORIG -median 4 $NEWBASE-median4$EXTENSION
convert $NEWORIG -median 5 $NEWBASE-median5$EXTENSION

# sharpen the image
convert $NEWORIG -sharpen 2x2 $NEWBASE-sharpen2$EXTENSION
convert $NEWORIG -sharpen 3x3 $NEWBASE-sharpen3$EXTENSION
convert $NEWORIG -sharpen 4x4 $NEWBASE-sharpen4$EXTENSION
convert $NEWORIG -sharpen 5x5 $NEWBASE-sharpen5$EXTENSION
convert $NEWORIG -sharpen 6x6 $NEWBASE-sharpen6$EXTENSION

# randomize a few images
convert $NEWORIG -spread 1 $NEWBASE-spread1$EXTENSION
convert $NEWORIG -spread 2 $NEWBASE-spread2$EXTENSION
convert $NEWORIG -spread 3 $NEWBASE-spread3$EXTENSION
convert $NEWORIG -spread 4 $NEWBASE-spread4$EXTENSION

# swirl images around
convert $NEWORIG -swirl 5 $NEWBASE-swirl5$EXTENSION
convert $NEWORIG -swirl 10 $NEWBASE-swirl10$EXTENSION
convert $NEWORIG -swirl 15 $NEWBASE-swirl15$EXTENSION
convert $NEWORIG -swirl 20 $NEWBASE-swirl20$EXTENSION
convert $NEWORIG -swirl 25 $NEWBASE-swirl25$EXTENSION
convert $NEWORIG -swirl -5 $NEWBASE-swirl-5$EXTENSION
convert $NEWORIG -swirl -10 $NEWBASE-swirl-10$EXTENSION
convert $NEWORIG -swirl -15 $NEWBASE-swirl-15$EXTENSION
convert $NEWORIG -swirl -20 $NEWBASE-swirl-20$EXTENSION
convert $NEWORIG -swirl -25 $NEWBASE-swirl-25$EXTENSION

# make wavy images
convert $NEWORIG -wave 3x3 $NEWBASE-wave3x3$EXTENSION

# now make some flipped images
for I in $( ls $OUTDIR ); do
    convert $OUTDIR/$I -flip $OUTDIR/$( basename -s $EXTENSION $I )-flipped$EXTENSION
done

# and now some flopped images
for I in $( ls $OUTDIR ); do
    convert $OUTDIR/$I -flop $OUTDIR/$( basename -s $EXTENSION $I )-flopped$EXTENSION
done

# NOTE: there are some ImageMagick filters that were tried but aren't useful
# edge: the ImageMagick edge detection filter, makes while outlines and the rest of the image becomes black
# charcoal: makes wierd outlines like bad edge detection
# paint: the oil paint filter, seems to be identical to the median blur
