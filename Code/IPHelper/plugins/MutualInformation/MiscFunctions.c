/* ============================================================================
* Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* Redistributions of source code must retain the above copyright notice, this
* list of conditions and the following disclaimer.
*
* Redistributions in binary form must reproduce the above copyright notice, this
* list of conditions and the following disclaimer in the documentation and/or
* other materials provided with the distribution.
*
* Neither the name of Michael A. Jackson nor the names of its contributors may
* be used to endorse or promote products derived from this software without
* specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
* USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
#error This file should NOT be compiled at all. This is just a holding area
for code that is not being used anymore but I still need to reference.
<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

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


int main(int argc, char **argv)
{
#if 1
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

}
