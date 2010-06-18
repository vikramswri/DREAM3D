/* ****************************************************************************
 * Original Author:
 * [1] Comer, Mary L., and Delp, Edward J.,  ÒThe EM/MPM Algorithm for Segmentation
 * of Textured Images: Analysis and Further Experimental Results,Ó IEEE Transactions
 *  on Image Processing, Vol. 9, No. 10, October 2000, pp. 1731-1744.
 *
 *  Adapted to current toolkit by Michael A. Jackson (aka BlueQuartz Software)
 *  <mike.jackson@bluequartz.net> under the U.S. Air Force contrct F33615-03-C-5220.
 *
 *  Initial port from original research codes performed by John van der Zwaag
 *  (vanderz@gri.msstate.edu).
 *
 * *************************************************************************** */

#ifndef _EM_MPM_RANDOM_H_
#define _EM_MPM_RANDOM_H_


#define VTK_K_A 16807
#define VTK_K_M 2147483647              /* Mersenne prime 2^31 -1 */
#define VTK_K_Q 127773                  /* VTK_K_M div VTK_K_A */
#define VTK_K_R 2836                    /* VTK_K_M mod VTK_K_A */

class Random
{
public:
  Random()
  {
    seed = 6157;
  }

  void setSeed(long s)
  {
    seed = s;
    next(); next(); next();
  }
  long getSeed()
  { return seed; }

  double next()
  {
    long hi, lo;

    // Based on code in "Random Number Generators: Good Ones are Hard to Find,"
    // by Stephen K. Park and Keith W. Miller in Communications of the ACM,
    // 31, 10 (Oct. 1988) pp. 1192-1201.
    // Borrowed from: Fuat C. Baran, Columbia University, 1988.
    hi = seed / VTK_K_Q;
    lo = seed % VTK_K_Q;
    if ((seed = VTK_K_A * lo - VTK_K_R * hi) <= 0)
      seed += VTK_K_M;
    return static_cast<double>(seed)/VTK_K_M;
  }

  double next(double min, double max)
  {
    return (min + next()*(max-min));
  }

protected:
  long seed;
};

#endif
