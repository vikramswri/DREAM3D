///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include <math.h>

#include "CMUMutualInformation.h"

#define GET_INDEX(x, y, z, nbins)\
  ((nbins * nbins * z) + (nbins * y) + x)

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CMUMutualInformation::CMUMutualInformation()
{
  // TODO Auto-generated constructor stub

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CMUMutualInformation::~CMUMutualInformation()
{
  // TODO Auto-generated destructor stub
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T, typename K>
class NotEqual
{
  public:
    bool operator() (T value, K compare)
    {
      return (value != compare);
    }
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<class T, typename K>
std::vector<size_t> where(AIMArray<uint32_t>::Pointer array, T operation, K value, int &count)
{
  std::vector<size_t> ret;
  count = 0;
  for (size_t i = 0; i < array->getNumElements(); ++i)
  {
    if ( operation(array->getValue(i), value ) )
    {
      ret.push_back(i);
      ++count;
    }
  }
  return ret;
}


int64_t factorial(int64_t num)
{
  int64_t result=1;
  for (int64_t i=1; i<=num; ++i)
     result *= i;
  return result;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int CMUMutualInformation::mutualInfomation(AIMArray<uint32_t>::Pointer ndhist, int level)
{
  level += 1;

  //get the dimensions to determine the sign of this contribution
  std::vector<size_t>::size_type szsz[1] = { ndhist->getDimensions().size() };

  int sign = 0;
  if (szsz[0] % 2 == 0)
  {
    sign = -1;
  }
  else
  {
    sign = +1;
  }

  // compute the entropy for this level of recursion
  int count;
  typedef NotEqual<uint32_t, float> NotEqualType;

  NotEqualType ne;
  float compare = 0.0f;
  std::vector<size_t> q = where(ndhist, ne, compare, count);
  //  q = where(ndhist ne 0.0,cnt)
  int64_t H;
  if (count != 0)
  //  if (cnt ne 0)
  {
    double total = 0;
    int64_t f = factorial(level);
    for (std::vector<size_t>::iterator iter = q.begin(); iter != q.end(); ++iter )
    {
      total += ndhist->getValue(*iter) * log( static_cast<double>(ndhist->getValue(*iter)) );
    }
    H = -sign * total/f;

    //    H = -sign*total(ndhist[q] * alog(ndhist[q]))/factorial(level)
  }
  else
  {
    std::cout << "Warning: All joint histogram entries are zero" << std::endl;
    return -1;
  }
//  else begin
//    print,'Warning: All joint histogram entries are zero'
//    return,-1
//  endelse
  std::cout << "Entropy contribution for level " << level << "  -> " << H  << std::endl;


  if (szsz[0] > 1) {
   for (int i=1; i < szsz[0]; ++i ) {
     H += mutualInfomation(reform(total(ndhist,i)), level);
     level -= 1;
   }
  }

  return H;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AIMArray<uint32_t>::Pointer CMUMutualInformation::jointHistogram(std::vector<ImageColumnType::Pointer> &inData,
                                                                 int binSize,
                                                                 bool normalize)
{
 // int err = NO_ERROR;

  size_t s[2] = {inData.size(), inData[0]->getNumElements() };
  // The check for a 2D array is implicit in the data structure that is passed in.


  int q = 0;
  int bsShifted = binSize >> q;
  while ( bsShifted != 1 )
  {
    q++;
    bsShifted = binSize >> q;
  }

  int32_t nbins = 256 >> q;

//  int32_t total_bins = pow (nbins, s[0]);

  AIMArray<uint32_t>::Pointer h = inData[s[0] -1]->ishft<uint32_t>( -q);
//  printf ("h     ");
//  h->printSelf(std::cout);
  AIMArray<uint32_t>::Pointer temp = AIMArray<uint32_t>::NullPointer();

  for (int i = s[0] - 2; i >= 0; --i)
  {
//    std::cout << "i = " << i << " q = " << q << " -------------------------------------------" << std::endl;
    h = h->ishft<uint32_t>( 8 - q);
//    printf ("ishft<uint32_t, uint32_t>(h, 8 - q)      ");
//    h->printSelf(std::cout);
    temp = inData[i]->ishft<uint32_t>( -q);
//    printf ("ishft<uint8_t, uint32_t>(inData[i], -q)  ");
//    temp->printSelf(std::cout);

    h = h->add<uint32_t, uint32_t>(temp);
    if (NULL == h.data())
    {
      return AIMArray<uint32_t>::NullPointer();
    }
//    printf ("add<uint32_t, uint32_t>(temp)            ");
//    h->printSelf(std::cout);
  }

  //ret=make_array(TYPE=3,DIMENSION=replicate(nbins,s[0]),/NOZERO)
  std::cout << "nbins: " << nbins << std::endl;
  std::cout << "s[0]:  " << s[0] << std::endl;

  std::vector<size_t> dimensions;
  size_t total_elements = 1;
  for (size_t i = 0; i < s[0]; ++i)
  {
    total_elements *= nbins;
    dimensions.push_back(nbins);
  }
  std::cout << "total_elements: " << total_elements << std::endl;
  AIMArray<uint32_t>::Pointer ret = AIMArray<uint32_t>::New();
  ret->allocateDataArray(total_elements, 1);
  ret->zeroArrayData();
  ret->setDimensions(dimensions);

  uint32_t* retPtr = ret->getPointer(0);
  uint32_t* hPtr = h->getPointer(0);
  for(size_t i = 0; i < h->getNumElements(); ++i)
  {
    retPtr[ hPtr[i] ]++;
  }

#if 0
  size_t index = GET_INDEX(1,2,3, nbins);
  std::cout << "index: " << index << "  ret[1,2,3]: " << retPtr[index] << std::endl;

  index = GET_INDEX(5,6,7, nbins);
  std::cout << "index: " << index << "  ret[5,6,7]: " << retPtr[index] << std::endl;

  index = GET_INDEX(9,10,11, nbins);
  std::cout << "index: " << index << "  ret[9,10,11]: " << retPtr[index] << std::endl;

  index = GET_INDEX(13,14,15, nbins);
  std::cout << "index: " << index << "  ret[13,14,15]: " << retPtr[index] << std::endl;

  index = GET_INDEX(0,0,0, nbins);
  std::cout << "index: " << index << "  ret[0,0,0]: " << retPtr[index] << std::endl;
#endif


  return ret;
}
