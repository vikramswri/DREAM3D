

#include "CMUMutualInformation.h"

#include <iostream>
#include <fstream>

#include <QtCore/QString>
#include <QtGui/QImage>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AIMArray<uint8_t>::Pointer convertQImageToGrayScaleAIMImage(QImage image)
{
  AIMArray<uint8_t>::Pointer aimImage = AIMArray<uint8_t>::New();
  quint8* oImage = aimImage->allocateDataArray(image.width() * image.height(), true);
  if (NULL == oImage)
  {
    return AIMArray<uint8_t>::NullPointer();
  }

  // Copy the QImage into the AIMImage object, converting to gray scale as we go.
  qint32 height = image.height();
  qint32 width = image.width();
  QRgb rgbPixel;
  int gray;
  qint32 index;
  for (qint32 y=0; y<height; y++) {
    for (qint32 x =0; x<width; x++) {
      index = (y *  width) + x;
      rgbPixel = image.pixel(x, y);
      gray = qGray(rgbPixel);
      oImage[index] = static_cast<unsigned char>(gray);
    }
  }
  return aimImage;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AIMArray<uint8_t>::Pointer loadImage(QString filePath)
{
  QImage image;
  AIMArray<uint8_t>::Pointer aimImage = AIMArray<uint8_t>::NullPointer();
  if (filePath.isEmpty() == false)
  {
    image = QImage(filePath);
    if (image.isNull() == true)
    {
      return aimImage;
    }
    QVector<QRgb> colorTable(256);
    for (quint32 i = 0; i < 256; ++i)
    {
      colorTable[i] = qRgb(i, i, i);
    }
    image.setColorTable(colorTable);
    aimImage = convertQImageToGrayScaleAIMImage(image);
    if (NULL == aimImage.data())
    {
      return aimImage;
    }
  }
  return aimImage;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T, typename K>
T getIndex_(T* S, const int ndims, K* i)
{
  T ndx = i[0];
  std::vector<T> products(ndims);
  products[0] = 1;
  for (int d = 1; d < ndims; ++d)
  {
    products[d] = products[d-1] * S[d-1];
    ndx = ndx + products[d] * i[d];
    if (i[d] >= S[d]) { printf ("ERROR; i[d]:%lu >= S[d]:%lu\n", i[d], S[d]); }
  }
  return ndx;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//template <typename T, typename K, typename J>
void printArray_(size_t ndims, size_t* S, size_t* i, size_t curDimIdx, int* n, int* array)
{
  if (0 == curDimIdx)
  {
    printf(" printArray: \n");
    printf("   ndims: %03lu\n", ndims);
    printf("\tS\tn\n");
    for (size_t a=0; a<ndims; ++a)
    {
      printf("\t%03lu\t%03d\n", S[a], n[a]);
    }
  }
  size_t ndx = 0;
  for (size_t d = 0; d < S[n[curDimIdx]]; ++d)
  {
    i[n[curDimIdx]] = d;
    if (curDimIdx == ndims-1)
    {
      ndx = getIndex_(S, ndims, i);
      printf("[%lu,%lu,%lu] %03d  ",i[0], i[1], i[2], array[ndx]);
      //printf("%03d  ", array[ndx]);
    }
    else
    {
      printArray_(ndims, S, i, curDimIdx+1, n, array);
    }
  }
  if (curDimIdx == ndims-1)
    printf("\n");

  if (curDimIdx == ndims-2)
    printf("------------------------------\n");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void calc_sums_(int* array,int actDim, int curDimIdx, size_t* dimsizes, int* n, size_t* S, int ndims, size_t* i,  int* totals, int &totalsIndex)
{
  size_t ndx = 0;
  for (size_t d = 0; d < dimsizes[curDimIdx]; ++d)
  {
    i[n[curDimIdx]] = d;
    if (curDimIdx == ndims - 2)
    {
      totals[totalsIndex] = 0;
    }
    if (actDim == n[curDimIdx])
    {
      ndx = getIndex_<size_t, size_t>(S, ndims, i);
      totals[totalsIndex] += array[ndx];
    }
    else
    {
      calc_sums_(array, actDim, curDimIdx+1, dimsizes, n, S, ndims, i, totals, totalsIndex);
    }
  }
  if (actDim == n[curDimIdx]) {
   // printf("%03d ", totals[totalsIndex]);
    totalsIndex++;
  }
 // if (curDimIdx == ndims - 2) printf("\n");
}

struct SumArray {
    int* data;
    size_t* dims;
    int ndims;
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void total_(int actDim, size_t* arrayDimSizes, int ndims, int* array, SumArray* output)
{
  size_t* i = static_cast<size_t*>(malloc(sizeof(size_t) * ndims));
  size_t* dimsizes = static_cast<size_t*>(malloc(sizeof(size_t) * ndims));
  int* dimIndexLUT = static_cast<int*>(malloc(sizeof(int) * ndims));

  size_t curIndex = 0;

  size_t tot = 1;
  for (int a = ndims - 1; a >= 0; --a)
  {
    if (a != actDim)
    {
      dimsizes[curIndex] = arrayDimSizes[a];
      tot *= arrayDimSizes[a];
      dimIndexLUT[curIndex] = a;
      ++curIndex;
    }
  }

  dimIndexLUT[curIndex] = (actDim);
  dimsizes[curIndex] = (arrayDimSizes[actDim]);
  if (NULL != output->data)
  {
    free(output->data);
    output->data = NULL;
  }
  if (NULL != output->dims)
  {
    free(output->dims);
    output->dims = NULL;
    output->ndims = 0;
  }

  int* sums = static_cast<int*>(malloc(sizeof(int) * tot));
  int sumsNDims = ndims-1;
  int sumsArrayIndex = 0;

  calc_sums_(array, actDim, 0, dimsizes, dimIndexLUT, arrayDimSizes, ndims, i, sums, sumsArrayIndex);

  // Print out the sums table
  size_t* sumsDimSizes = static_cast<size_t*>(malloc(sizeof(size_t) * sumsNDims));
  int* sumsDimLUT = static_cast<int*>(malloc(sizeof(int) * sumsNDims));

  for (int a = sumsNDims-1; a >= 0; --a)
  {
    sumsDimSizes[a] = dimsizes[sumsNDims-1-a];
    sumsDimLUT[a] = sumsNDims-1-a;
  }
//  printArray_(sumsNDims, sumsDimSizes, i, 0, sumsDimLUT, sums);

  output->data = sums;
  output->dims = sumsDimSizes;
  output->ndims = sumsNDims;


//  free(sums);
  free(i);
  free(dimIndexLUT);
//  free(sumsDimSizes);
  free(sumsDimLUT);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int main(int argc, char **argv)
{
  std::cout << "Mutual Information Test" << std::endl;

#if 0
  size_t ndims = 3;
  size_t* S = static_cast<size_t*>(malloc(sizeof(size_t) * ndims)); // Allocate for 3 dimensions
  S[0] = 5;
  S[1] = 4;
  S[2] = 3;
  size_t* i = static_cast<size_t*>(malloc(sizeof(size_t) * ndims)); // Allocate for 3 dimensions
  i[0] = 0;
  i[1] = 0;
  i[2] = 0;

  int* array = static_cast<int*>( malloc(sizeof(int) * S[0] * S[1] * S[2] ) );
  size_t ndx = 0;
  for (size_t z = 0; z < S[2]; ++z)
  {
   // printf("\n z=%lu\n", z);
    for (size_t y = 0; y < S[1]; ++y)
    {
      for (size_t x = 0; x < S[0]; ++x)
      {
        i[0] = x; i[1] = y; i[2] = z;
        ndx = getIndex_<size_t, size_t>(S, ndims, i);
        array[ndx] = ndx;
     //   printf("[%lu,%lu,%lu] %lu  ",i[0], i[1], i[2], ndx);
      }
     // printf("\n");
    }

  }
 // printf("+++++++++++++++++++++++++++++++++++++++\n");
  ndims = 3;
  size_t curDim = 0;
  int actDim = 0;
  i[0] = 0;
  i[1] = 0;
  i[2] = 0;
  printf("Input Array Size: [ ");
  for (size_t a = 0; a < ndims; ++a)
  {
    printf("%lu ", S[a]);
  }
  printf("]\n");


  int n[3] = {2, 1, 0};
  printArray_(ndims, S, i, curDim, n, array);

  SumArray* output = static_cast<SumArray*>(malloc(sizeof(SumArray)));
  output->data = NULL;
  output->dims = NULL;
  output->ndims = 0;
  total_(actDim, S, ndims, array, output);
  actDim = 1;
  total_(actDim, S, ndims, array, output);
  actDim = 2;
  total_(actDim, S, ndims, array, output);

  free(output->data);
  free(output->dims);
  free(output);
  free(S);
  free(i);
  free(array);

  if (true) exit(-1);
#endif

  std::vector<AIMArray<uint8_t>::Pointer> images;

  AIMArray<uint8_t>::Pointer i0 = loadImage(QString("/Users/mjackson/Contracts/AFRL-TO79/Mutual_Info_CMU/6044.tif"));
  AIMArray<uint8_t>::Pointer i1 = loadImage(QString("/Users/mjackson/Contracts/AFRL-TO79/Mutual_Info_CMU/5044.tif"));
  images.push_back(i0);
  images.push_back(i1);


#if 0
  unsigned char v0[4] =
  { 1,5,9,13};
  unsigned char v1[4] =
  { 2,6,10,14};
  unsigned char v2[4] =
  { 3,7,11,15};

  ImageColumnType::Pointer V0 = ImageColumnType::New();
  V0->initializeImageWithSourceData(4, v0);

  ImageColumnType::Pointer V1 = ImageColumnType::New();
  V1->initializeImageWithSourceData(4, v1);

  ImageColumnType::Pointer V2 = ImageColumnType::New();
  V2->initializeImageWithSourceData(4, v2);

  images.push_back(V0);
  images.push_back(V1);
  images.push_back(V2);
#endif


  int binSize = 1;

  CMUMutualInformation mi;

  AIMArray<uint32_t>::Pointer jointHistogram = mi.jointHistogram(images, binSize, false);
  AIMArray<double>::Pointer normalizedJointHistogram = jointHistogram->normalize<double>();
  double min, max;
  normalizedJointHistogram->minMax(min, max);
  std::cout << "Normalized Histogram Min/Max: " << min << "  " << max << std::endl;
  int level = -1;
  double H = mi.mutualInfomation(normalizedJointHistogram, level);
  std::cout << "Mutual Information: " << H << std::endl;

  std::ofstream ofile("/tmp/joinhistogram.raw", std::ios::binary);
  size_t length = jointHistogram->getNumElements();
  ofile.write( reinterpret_cast<char *>(jointHistogram->getPointer(0)), length * 4);
  ofile.flush();

  std::cout << "Done" << std::endl;
  return EXIT_SUCCESS;
}
