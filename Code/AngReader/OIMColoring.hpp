///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
//  Copyright (c) 2010, Michael Groeber. US Air Force
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef OIMCOLORING_HPP_
#define OIMCOLORING_HPP_


#include <math.h>
#include <string>

#define MXA_PI          3.141592653589793
#define MXA_PI_OVER_4   0.785398163397448

/**
* @class OIMColoring OIMColoring.hpp Server/IPFFilter/OIMColoring.hpp
* @brief This class helps generate an RGB color based on a Euler Angle and a
* Reference Direction.
* @author Michael A. Jackson for BlueQuartz Software
* @author Michael Groeber for the US Air Force
* @date Feb 24, 2010
* @version 1.0
*/
class OIMColoring
{

  public:
   static std::string  EulerAngles() { return std::string("Euler_Angles"); }

  /**
   * @brief Sorts the 3 values from low to high
   * @param a
   * @param b
   * @param c
   * @param sorted The array to store the sorted values.
   */
    template<typename T>
    static void TripletSort(T a, T b, T c, T* sorted)
    {
      if ( a > b && a > c)
      {
        sorted[2] = a;
        if (b > c) { sorted[1] = b; sorted[0] = c; }
        else { sorted[1] = c; sorted[0] = b; }
      }
      else if ( b > a && b > c)
      {
        sorted[2] = b;
        if (a > c) { sorted[1] = a; sorted[0] = c; }
        else { sorted[1] = c; sorted[0] = a; }
      }
      else if ( a > b )
      {
        sorted[1] = a; sorted[0] = b; sorted[2] = c;
      }
      else if (a >= c && b >=c)
      {
        sorted[0] = c; sorted[1] = a; sorted[2] = b;
      }
      else
      { sorted[0] = a; sorted[1] = b; sorted[2] = c;}
    }

/**
 * @brief Returns the maximum value of the three values
 * @param red
 * @param green
 * @param blue
 * @return
 */
    template<typename T>
    static T TripletMax(T red, T green, T blue)
    {
      if ( red > green && red > blue)
      {
        return red;
      }
      else if ( green > red && green > blue)
      {
        return green;
      }
      return blue;
    }


    /**
     * @brief returns the minimum of the three values
     * @param red
     * @param green
     * @param blue
     * @return
     */
    template<typename T>
    static T TripletMin(T red, T green, T blue)
    {
      if ( red < green && red < blue)
      {
        return red;
      }
      else if ( green < red && green < blue)
      {
        return green;
      }
      return blue;
    }



    /**
     * @brief Wrapper for convenience
     * @param eulers The euler angles which MUST be encode into the array in the following order:
     * phi1, Phi, phi2
     * @param refDir The Reference direction. Usually either the ND (001), RD(100), or TD(010)
     * @param rgb A pointer to store the RGB value into
     */
    template<typename T>
    static void GenerateIPFColor(T* eulers, T* refDir, unsigned char* rgb)
    {
      OIMColoring::GenerateIPFColor<T>(eulers[0], eulers[1], eulers[2],
                                       refDir[0], refDir[1], refDir[2],rgb);
    }

    /**
     * @brief Generates an RGB color based on the Inverse Pole Figure coloring
     * @param phi1 The phi1 euler Angle
     * @param phi The Phi euler Angle
     * @param phi2 The phi2 euler Angle
     * @param refDir0 The first component of the Reference direction vector
     * @param refDir1 The Second component of the Reference direction vector
     * @param refDir2 The third component of the Reference direction vector
     * @param rgb A pointer to store the RGB value into
     */
    template <typename T>
    static void GenerateIPFColor(T phi1, T phi, T phi2,
                                 T refDir0, T refDir1, T refDir2,
                                 unsigned char* rgb)
    {
     // const T SqrtTwo = static_cast<T>(sqrt(2.0));
      const T SqrtTwoOverTwo = static_cast<T>(sqrt(2.0)/2.0);
      const T One80OverPi = static_cast<T>(180.0/MXA_PI);
      const T Ninety = 90;
      const T FortyFive = 45;
      const T ThirtySomething = static_cast<T>(35.26);

      T go[3][3]; // Rotation Matrix?
      T cd[3];
      T sd[3];
      T d[3];

      // Calculate all the values once
      T cos_phi1 = cosf(phi1);
      T sin_phi1 = sinf(phi1);
      T cos_phi = cosf(phi);
      T sin_phi = sinf(phi);
      T cos_phi2 = cosf(phi2);
      T sin_phi2 = sinf(phi2);

      // 1) find rotation matrix from Euler angles
      go[0][0] = cos_phi1 * cos_phi2 - sin_phi1 * sin_phi2 * cos_phi;
      go[0][1] = sin_phi1 * cos_phi2 + cos_phi1 * sin_phi2 * cos_phi;
      go[0][2] = sin_phi2 * sin_phi;
      go[1][0] = -cos_phi1 * sin_phi2 - sin_phi1 * cos_phi2 * cos_phi;
      go[1][1] = -sin_phi1 * sin_phi2 + cos_phi1 * cos_phi2 * cos_phi;
      go[1][2] = cos_phi2 * sin_phi;
      go[2][0] = sin_phi1 * sin_phi;
      go[2][1] = -cos_phi1 * sin_phi;
      go[2][2] = cos_phi;


      // 2) use rotation matrix to find which crystal direction is aligned with 001
      cd[0] = go[0][0] * refDir0 + go[0][1] * refDir1 + go[0][2] * refDir2;
      cd[1] = go[1][0] * refDir0 + go[1][1] * refDir1 + go[1][2] * refDir2;
      cd[2] = go[2][0] * refDir0 + go[2][1] * refDir1 + go[2][2] * refDir2;
    // IDL Code to here.
      //3) move that direction to a single standard triangle - using the 001-011-111 triangle)
      sd[0] = fabs(cd[0]);
      sd[1] = fabs(cd[1]);
      sd[2] = fabs(cd[2]);

      // Sort the sd array from smallest to largest
      OIMColoring::TripletSort<T>(sd[0], sd[1], sd[2], sd);

      T theta = (sd[0]*0)+(sd[1]*-SqrtTwoOverTwo)+(sd[2]*SqrtTwoOverTwo);
      theta = static_cast<T>((One80OverPi))*acos(theta);
      T red = (Ninety-theta)/FortyFive;
      d[0] = (sd[1]*1)-(sd[2]*0);
      d[1] = (sd[2]*0)-(sd[0]*1);
      d[2] = (sd[0]*0)-(sd[1]*0);
      d[0] = -(d[1]+d[2])/d[0];
      d[1] = 1;
      d[2] = 1;
      T norm =  static_cast<T>(powf(((d[0]*d[0])+(d[1]*d[1])+(d[2]*d[2])),0.5));
      d[0] = d[0]/norm;
      d[1] = d[1]/norm;
      d[2] = d[2]/norm;
      T phi_local = (d[0]*0)+(d[1]*SqrtTwoOverTwo)+(d[2]*SqrtTwoOverTwo);
      phi_local = static_cast<T>(One80OverPi)*acos(phi_local);
      T green = (1-red)*((ThirtySomething-phi_local)/ThirtySomething);
      T blue = (1-red)-green;
      T max = red;
      if(green > max) max = green;
      if(blue > max) max = blue;

      // Scale values from 0 to 1.0
      red = red/max;
      green = green/max;
      blue = blue/max;

      // Add in some correction factors
      red = static_cast<T>((0.75*red)+0.25);
      green = static_cast<T>((0.75*green)+0.25);
      blue = static_cast<T>((0.75*blue)+0.25);

      // Multiply by 255 to get an R/G/B value
      red = red * 255.0f;
      green = green * 255.0f;
      blue = blue * 255.0f;

      rgb[0] = static_cast<unsigned int>(red);
      rgb[1] = static_cast<unsigned int>(green);
      rgb[2] = static_cast<unsigned int>(blue);
    }


  protected:
    OIMColoring() {};
    ~OIMColoring() {};


  private:
    OIMColoring(const OIMColoring&);    // Copy Constructor Not Implemented
    void operator=(const OIMColoring&);  // Operator '=' Not Implemented

};




#endif /* OIMCOLORING_HPP_ */
