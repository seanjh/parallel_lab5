# Parallel Labs 4 & 5

## Instructions

    make filter_image
    ./filter_image source_ppm stencil_pgm iterations

## Stencils
Image stencils should be represented in [Netbpm format](https://en.wikipedia.org/wiki/Netpbm_format), specifically as Portable GrayMap ASCII (P2). In these PGMs, the stencil values range from 0-255. These values are translated to a range between -8 and 8 before the stencil is applied to the image (see below).

### Stencil-PGM value equivalents

| Stencil Values    | PGM Value   |
|:-----------------:|:-----------:|
| -8                | 0           |
| -7                | 16          |
| -6                | 32          |
| -5                | 48          |
| -4                | 64          |
| -3                | 80          |
| -2                | 96          |
| -1                | 112         |
|  0                | 128         |
|  1                | 143         |
|  2                | 159         |
|  3                | 175         |
|  4                | 191         |
|  5                | 207         |
|  6                | 223         |
|  7                | 239         |
|  8                | 255         |

### Normalization
Stencils are normalized using one of two methods. In cases where the sum of the stencil values (converted from the PGM values) is not 0, then the stencil values are all normalized to sum up to 1.0. When the stencil values (before normalization) sum up to 0, then all the negative values are normalized to sum up to -1.0, and the positive values are normalized to sum up to 1.0.

## Requirements
### Lab 4+5: Shared memory parallelism

### Part 1: Analysis
The goal of this part of the lab is do analyze a few standard ways to parallelize an application in terms of computation and communication cost.

Consider a regular grid of values, where the computation is described using a stencil of size 2K-1 x 2K-1.If the computation domain (size N x N) is divided up into rectangular blocks of size X x Y, and the ghost boundary approach is used for parallelization, what is the ratio of computation time to communication time for an iteration? How would you select X and Y to optimize performance? (You may assume that N is divisible by X and Y to make things simpler.)
We came up with a model for communication cost in a message-passing machine based on latency and throughput. Develop a  model for the communication cost in a shared memory machine, and explain how your model can be used to estimate performance.

### Part 2: Shared memory parallelism
The goal of this part of the lab is to implement a shared memory parallel program using OpenMP. The problem you will implement is parallel image filtering, where a stencil is applied to a very large image. Your program should read in an image and stencil, apply the stencil repeatedly (the repeat count should be an argument to the program), and write out the final image to the output.

An image is simply a 2D array of (R,G,B) values, where (R,G,B) is the "red", "blue", and "green" component of the pixel value. Often these values are integers ranging from 0 to 255, but other options are also possible.
Your program should read and write images in the PPM file format. This format is very straightforward to read and write, especially if you use the ASCII version (which is fine).
The "netpbm" package is an open source package that contains converters to/from PPM to almost any standard image format (such as JPEG, PNG, etc.) and can be used to create inputs and view outputs from your program.
A stencil can also be viewed as a grayscale image, and so should be specified using the PGM format (see the link for PPM). Useful stencils have positive as well as negative values, while PGM files can only have integer inputs from 0 to M-1, where M is specified in the file. To handle this, convert the PGM value p to the stencil value -4+(8*p)/(M-1). This means that the PGM value 0 corresponds -4, and the PGM value (M-1) corresponds to +4.
If you want some examples of interesting stencils, check out the following:

* [Blurring an image](http://en.wikipedia.org/wiki/Gaussian_blur)
* [Edge detection](http://en.wikipedia.org/wiki/Sobel_operator)

Your program should be parameterized by the number of threads (i.e. make sure it works for an arbitrary number of threads).