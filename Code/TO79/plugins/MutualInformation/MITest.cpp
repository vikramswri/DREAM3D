

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
T getIndex(T* S, const int ndims, K* i)
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
void printArray(size_t ndims, size_t* S, size_t* i, size_t curDimIdx, int* n, int* array)
{
  size_t ndx = 0;
  for (size_t d = 0; d < S[n[curDimIdx]]; ++d)
  {
    i[n[curDimIdx]] = d;
    if (curDimIdx == ndims-1)
    {
      ndx = getIndex(S, ndims, i);
      printf("[%lu,%lu,%lu] %03d  ",i[0], i[1], i[2], array[ndx]);
      //printf("%02d  ", array[ndx]);
    }
    else
    {
      printArray(ndims, S, i, curDimIdx+1, n, array);
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
void total_summation(int actDim, int curDimIdx, int* dimsizes, int* n, size_t* S, int ndims, size_t* i, int* array, int* totals, int &totalsIndex)
{
  size_t ndx = 0;
  for (int d = 0; d < dimsizes[curDimIdx]; ++d)
  {
    i[n[curDimIdx]] = d;
    if (curDimIdx == ndims - 2)
    {
      totals[totalsIndex] = 0;
    }
    if (actDim == n[curDimIdx])
    {
      ndx = getIndex<size_t, size_t>(S, ndims, i);
      totals[totalsIndex] += array[ndx];
    }
    else
    {
      total_summation(actDim, curDimIdx+1, dimsizes, n, S, ndims, i, array, totals, totalsIndex);
    }
  }
  if (actDim == n[curDimIdx]) {
    totalsIndex++;
    printf("%03d ", totals[totalsIndex]);
  }
  if (curDimIdx == ndims - 2) printf("\n");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void total(int actDim, size_t* S, int ndims, size_t* i, int* array)
{
  // Compute the number and size of the dimensions of the return
  size_t sDimSizes[3] = {0, 0, 0};
  int dimsizes[3] = { 0, 0, 0};
  int n[3] = { 0, 0, 0};
  size_t curIndex = 0;

  size_t tot = 1;
  for (int a = ndims - 1; a >= 0; --a)
  {
    if (a != actDim)
    {
      dimsizes[curIndex] = (S[a]);
      tot *= S[a];

      n[curIndex] = (a);
      ++curIndex;
    }
  }

  printf("Output Array will be sized: [ ");
  for (int a = ndims-2; a >= 0; --a)
  {
    printf("%d ", dimsizes[a]);
    sDimSizes[a] = dimsizes[a];
  }
  printf("]\n");
  n[curIndex] = (actDim);
  dimsizes[curIndex] = (S[actDim]);
  int* totals = static_cast<int*>(malloc(sizeof(int) * tot));
  int totalsIndex = 0;
 // int sum = 0;

  total_summation(actDim, 0, dimsizes, n, S, ndims, i, array, totals, totalsIndex);


//  printArray(ndims-1, sDimSizes, i, ndims - 2, actDim, totals);


  free(totals);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int main(int argc, char **argv)
{
  std::cout << "Mutual Information Test" << std::endl;
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
        ndx = getIndex<size_t, size_t>(S, ndims, i);
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
  printArray(ndims, S, i, curDim, n, array);

  total(actDim, S, ndims, i, array);
  actDim = 1;
  total(actDim, S, ndims, i, array);
  actDim = 2;
  total(actDim, S, ndims, i, array);

  free(S);
  free(i);
  free(array);

  if (true) exit(-1);


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
  mi.mutualInfomation(jointHistogram);

  std::ofstream ofile("/tmp/joinhistogram.raw", std::ios::binary);
  size_t length = jointHistogram->getNumElements();
  ofile.write( reinterpret_cast<char *>(jointHistogram->getPointer(0)), length * 4);
  ofile.flush();

  std::cout << "Done" << std::endl;
  return EXIT_SUCCESS;
}
