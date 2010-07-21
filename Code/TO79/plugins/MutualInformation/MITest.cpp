

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
size_t getIndex(size_t* S, const size_t ndims, size_t* i)
{
  size_t ndx = i[0];
  std::vector<size_t> products(ndims);
  products[0] = 1;
  for (size_t d = 1; d < ndims; ++d)
  {
    products[d] = products[d-1] * S[d-1];
    ndx = ndx + products[d] * i[d];
    if (i[d] >= S[d]) { printf ("ERROR; i[d]:%lu >= S[d]:%lu\n", i[d], S[d]); }
  }
  return ndx;
}

void printArray(size_t ndims, size_t* S, size_t* i, size_t curDim, size_t actDim, int* array)
{
  size_t ndx = 0;
  for (size_t d = 0; d < S[curDim]; ++d)
  {
    i[curDim] = d;
    if (curDim == 0)
    {
      ndx = getIndex(S, ndims, i);
      //printf("[%lu,%lu,%lu] %d  ",i[0], i[1], i[2], array[ndx]);
      printf("%02d  ", array[ndx]);
    }
    else
    {
      printArray(ndims, S, i, curDim-1, actDim, array);
    }
  }
  if (curDim == 0)
    printf("\n");

  if (curDim == ndims - 2)
    printf("------------------------------\n");
}


void sumActiveDimension(size_t ndims, size_t* S, size_t* i, size_t curDim, size_t actDim, int* array)
{
  size_t ndx = 0;
  for (size_t d = 0; d < S[curDim]; ++d)
  {
    i[curDim] = d;

    if (curDim == 0)
    {
      ndx = getIndex(S, ndims, i);
      printf("[%lu,%lu,%lu] %d  ",i[0], i[1], i[2], array[ndx]);
    }
    else
    {
      sumActiveDimension(ndims, S, i, curDim-1, actDim, array);
    }
  }
  if (curDim == 0)
  printf("\n");

  if (curDim == ndims - 2)
  printf("------------------------------\n");
}



void total(size_t actDim, size_t* S, size_t ndims, size_t* i, int* array)
{
  // Compute the number and size of the dimensions of the return
  std::vector<int> dimsizes;
  std::vector<int> n;
  printf("Output Array will be sized: [");
  size_t tot = 1;
  for (size_t a = 0; a < ndims; ++a)
  {
    if (a != actDim)
    {
      dimsizes.push_back(S[a]);
      tot *= S[a];
      printf("%lu ", S[a]);
      n.push_back(a);
    }
  }
  printf("]\n");
  n.push_back(actDim);
  dimsizes.push_back(S[actDim]);
  size_t retNDims = dimsizes.size();
  int* totals = static_cast<int*>(malloc(sizeof(int) * tot));
  int sum = 0;
  size_t ndx = 0;

  for (int d = 0; d < dimsizes[1]; ++d)
  {

    for (int dd = 0; dd < dimsizes[0]; ++dd)
    {
      sum = 0;
      for (int ddd = 0; ddd < dimsizes[2]; ++ddd)
      {
        i[n[1]] = d;
        i[n[0]] = dd;
        i[n[2]] = ddd;
        ndx = getIndex(S, ndims, i);
        sum += array[ndx];
      }
      printf("%03d ", sum);
    }
    printf("\n");
  }


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
        ndx = getIndex(S, ndims, i);
        array[ndx] = ndx;
     //   printf("[%lu,%lu,%lu] %lu  ",i[0], i[1], i[2], ndx);
      }
     // printf("\n");
    }

  }
 // printf("+++++++++++++++++++++++++++++++++++++++\n");
  ndims = 3;
  size_t curDim = 2;
  size_t actDim = 0;
  i[0] = 0;
  i[1] = 0;
  i[2] = 0;
  printArray(ndims, S, i, curDim, actDim, array);

  total(actDim, S, ndims, i, array);
  actDim = 1;
  total(actDim, S, ndims, i, array);
  actDim = 2;
  total(actDim, S, ndims, i, array);


 // total(ndims, S, i, curDim, actDim, array);


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
