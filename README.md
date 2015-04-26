# Parallel Labs 4 & 5


## Stencils
Image stencils are all represented in [Netbpm format](https://en.wikipedia.org/wiki/Netpbm_format), specifically as Portable GrayMap ASCII (P2). In these PGMs, the stencil values range from 0-255. These values are translated to a range between -8 and 8 before the stencil is applied to the image, as shown below.

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