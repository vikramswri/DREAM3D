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

  size_t total_elements = 1;
  for (size_t i = 0; i < s[0]; ++i)
  {
    total_elements *= nbins;
  }
  std::cout << "total_elements: " << total_elements << std::endl;
  AIMArray<uint32_t>::Pointer ret = AIMArray<uint32_t>::New();
  ret->allocateDataArray(total_elements, 1);
  ret->zeroArrayData();
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
