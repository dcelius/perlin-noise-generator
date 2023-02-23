# perlin-noise-generator

Written by Dylan Celius

## Summary
perlin_ppm_maker takes in the filename of an input file to generate a ppm file with the corresponding specifications. 

## Input File Parameters
imsize [width] [height] <br>
color [r] [g] [b] <br>
freq [frequency]

## Restrictions:
- Currently, width and height must be equal - the program relies on a square input
- At larger sizes, the program becomes increasingly unstable. This seems to begin at values greater than 3000
- The RGB values provided in color must be in the range of 0-255
- The integer used for freq must be postive

## Notes
- imsize is required, the other values will default if not provided.
- The general output will be a ppm image consisting of perlin noise. 
- The primary color will be the one provided in the text file. 
- In addition, a complementary color is determined from the primary color as well as an intermediate color. 
- The frequency will determine how rough the edges of the perlin noise is. 
- Each image generation should be unique, as the random generator is seeded with the current system time.

## Compilation
The program can be compiled using: <br>
make all

The program can be run using: <br>
./perlin_ppm_maker [input file]
