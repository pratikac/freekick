//==============================================================================
// $Id: random.h 115 2009-06-10 21:39:36Z kartikmohta $
//==============================================================================

#ifndef _RANDOM_H
#define _RANDOM_H

//use this first function to seed the random number generator,
//call this before any of the other functions
void initrand();

//generates a psuedo-random integer between 0 and 32767
int randint();

//generates a psuedo-random integer between 0 and max
int randint(int max);

//generates a psuedo-random integer between min and max
int randint(int min, int max);

//generates a psuedo-random float between 0.0 and 0.999...
float randfloat();

//generates a psuedo-random float between 0.0 and max
float randfloat(float max);

//generates a psuedo-random float between min and max
float randfloat(float min, float max);

//generates a psuedo-random double between 0.0 and 0.999...
double randdouble();

//generates a psuedo-random double between 0.0 and max
double randdouble(double max);

//generates a psuedo-random double between min and max
double randdouble(double min, double max);

#endif
