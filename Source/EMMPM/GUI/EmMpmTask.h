///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////
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

#ifndef _EMMPM_TASK_H_
#define _EMMPM_TASK_H_

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <QtCore/QObject>
#include <QtCore/QThread>
#include <QtCore/QString>
#include <QtGui/QImage>

#include "EMMPM/Common/AIMImage.h"
#include "EMMPM/Common/Random.h"

#define PI  3.14159265358979323846
#define MAX_CLASSES 15

#define MAXPRIME  2147483647       /*  MAXPRIME = (2^31)-1     */

#define UPDATE_PROGRESS(m, p)\
  emit progressTextChanged( (m) );\
  emit progressValueChanged( (p) );

/**
* @class EmMpmTask EmMpmTask.h EmMpm/GUI/EmMpmTask.h
* @brief THis is the wrapper code for the code. This is called as a "worker" class from a separate thread
* of execution in order to not lock up the GUI.
* @author Michael A. Jackson for BlueQuartz Software
* @date Dec 20, 2009
* @version 1.0
*/
class EmMpmTask : public QThread
{

  Q_OBJECT

  public:
    EmMpmTask(QObject* parent = 0);
    virtual ~EmMpmTask();


    /**
     * @brief Is the task canceled
     * @return
     */
    bool isCanceled();

    MXA_INSTANCE_PROPERTY_m(bool, Debug);

    MXA_INSTANCE_PROPERTY_m(QString, InputFilePath);
    MXA_INSTANCE_PROPERTY_m(QString, OutputFilePath);
    void setBeta(float beta);
    void setGamma(float gamma);
    void setEmIterations(int emIterations);
    void setMpmIterations(int mpmIterations);
    void setNumberOfClasses(int numClasses);
    void useSimulatedAnnealing();

    /**
     *
     * @param imageData
     * @param outputData
     */
    template <class T, class S>
    void execute( T* imageData, S* outputData);

    /**
     *
     * @param imageData
     * @param segmentation
     * @param pr
     * @param yk
     * @param beta
     * @param gamma
     * @param mean_estimate
     * @param variance
     * @param idim
     * @param jdim
     * @param random
     */
    template <class T, class S>
    void mpm( T* imageData, S* segmentation,
                           double *pr[], double *yk[],
                           double beta, double *gamma,
                           double *mean_estimate, double *variance,
                           size_t idim, size_t jdim,
                           Random &random);


  signals:

  /**
   * @brief Signal sent when the encoder has a message to relay to the GUI or other output device.
   */
    void progressTextChanged ( const QString & progressText );

    /**
     * @brief Signal sent to the GUI to indicate progress of the encoder which is an integer value between 0 and 100.
     * @param value
     */
    void progressValueChanged(int value);

    /**
     * @brief Signal sent when the encoding task is complete
     */
    void finished();
    void finished(QObject *);

  public slots:

    /**
     * @brief Slot to receive a signal to cancel the operation
     */
    void cancel();

    virtual void run();

  protected:
    AIMImage::Pointer convertQImageToGrayScaleAIMImage(QImage image);


  private:
    bool m_Cancel;
    float m_Beta;
    float m_Gamma;
    int m_EmIterations;
    int m_MpmIterations;
    int m_NumberOfClasses;
    bool m_UseSimulatedAnnealing;
    int m_TotalIterations;
    int m_CurrentIteration;

    AIMImage::Pointer m_OriginalImage;
    AIMImage::Pointer m_SegmentedImage;

    EmMpmTask(const EmMpmTask&); // Copy Constructor Not Implemented
    void operator=(const EmMpmTask&); // Operator '=' Not Implemented

};

template <class T, class S>
void EmMpmTask::mpm( T* imageData, S* segmentation,
                       double *pr[], double *yk[],
                       double beta, double *gamma,
                       double *mean_estimate, double *variance,
                       size_t idim, size_t jdim,
                       Random &random)
{
  double con[MAX_CLASSES], d[MAX_CLASSES], post[MAX_CLASSES];
  size_t prior[MAX_CLASSES];
  size_t ijdim = idim*jdim;
  size_t extent_ijdim = ijdim;
  double sqrt2pi = ::sqrt(PI * 2);
  int components = 1;

  uint32_t NumberClasses = m_NumberOfClasses;
  uint32_t MPMIterations = m_MpmIterations;
  for (uint32_t l = 0; l < NumberClasses; ++l)
  {
    con[l] = -::log(sqrt2pi * ::sqrt(variance[l]));
    d[l] = -2.0 * variance[l];
  }

  int index = 0;

  for (size_t i = 0; i < ijdim; ++i)
  {
    for (uint32_t l = 0; l < NumberClasses; ++l)
    {
      pr[l][index] = 0;
      yk[l][index] = con[l] + ((imageData[i] - mean_estimate[l]) * (imageData[i] - mean_estimate[l])/d[l]);
    }
    ++index;
  }


  for (uint32_t k = 0; k < MPMIterations; ++k)
  {
    if (m_Cancel == true) { break; }
    ++m_CurrentIteration;
    int progress = (float)m_CurrentIteration/(float)m_TotalIterations * 100.0f;
    emit progressValueChanged( progress );
    emit progressTextChanged(QString::number(progress));
    int extent_index = 0;
    for (size_t j = 0; j < jdim; ++j)
    {
      for (size_t i = 0; i < idim; ++i)
      {
        size_t index = j*idim*components+i*components;

        double sum = 0;
        for (uint32_t l = 0; l < NumberClasses; ++l)
        {
          prior[l] = 0;
          // 8 point neighborhood
          if (i > 0) // left
            if (segmentation[index-components] != l)
              prior[l]++;
          if (i < idim-1) // right
            if (segmentation[index+components] != l)
              prior[l]++;
          if (j > 0) // bottom
            if (segmentation[index-idim*components] != l)
              prior[l]++;
          if (j < jdim-1) // top
            if (segmentation[index+idim*components] != l)
              prior[l]++;
          if (i > 0 && j > 0) // bottom left
            if (segmentation[index-idim*components-components] != l)
              prior[l]++;
          if (i > 0 && j < jdim-1) // top left
            if (segmentation[index+idim*components-components] != l)
              prior[l]++;
          if (i < idim-1 && j > 0) // bottom right
            if (segmentation[index-idim*components+components] != l)
              prior[l]++;
          if (i < idim-1 && j < jdim-1) // bottom left
            if (segmentation[index+idim*components+components] != l)
              prior[l]++;

          post[l] = exp(yk[l][extent_index] - beta * (double)(prior[l]) - gamma[l]);
          sum += post[l];
        }
        double x = random.next();
        double current = 0;
        for (uint32_t l = 0; l < NumberClasses; ++l)
        {
          if (x >= current && x <= current+post[l]/sum)
          {
            segmentation[index] = l;//index] = l;
            pr[l][extent_index] += 1.0;
          }
          current += post[l]/sum;
        }
        ++extent_index;
      }
    }
  }
  if (m_Cancel == false)
  {
    // normalize probabilities
    for (uint32_t l = 0; l < NumberClasses; ++l)
    {
      for (size_t i = 0; i < extent_ijdim; ++i)
      {
        pr[l][i] = pr[l][i] / (double)MPMIterations;
      }
    }
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <class T, class S>
void EmMpmTask::execute( T* imageData, S* outputData)
{
  // data dimensions
  int32_t width, height;
  m_OriginalImage->getImagePixelSize(width, height);
  size_t idim = width;
  size_t jdim = height;
  size_t ijdim = idim*jdim;
  size_t extent_ijdim = idim*jdim;

  // output pixels
  S *xt = outputData;
  Random random;
 // int components = 1;


  // ==========================================================================
  // Compute the Mean and Variance in one loop using a Maximum Liklehood Algo
  uint8_t t;
  double local_mu = 0.0;
  double local_sigma = 0.0;
  int32_t s0;
  int64_t s1, s2 = 0;
  s0 = 0;
  s1 = 0;
  for (size_t i = 0; i < ijdim; ++i)
  {
    t = imageData[i];
    s0 += t != 0; // Count number of non-zero elements.
    s1 += t;    // Accumulate sum.
    s2 += t*t;    // Accumulate sum of squares.
  }

  local_mu = (double) s1 / s0;
  local_sigma = (s2 - s1*local_mu)/s0;
  local_sigma = sqrt(local_sigma);

  // initialize variables on main thread
  double local_mean_estimate[MAX_CLASSES];
  double local_variance[MAX_CLASSES];
  int NumberClasses = m_NumberOfClasses;
  if (true)//threadId == 0)
  {
    //fprintf(stderr, "mu=%f sigma=%f\n", local_mu, local_sigma);
    // initialize mean estimate
    if (NumberClasses % 2 == 0)
    {
      for (int k = 0; k < NumberClasses/2; ++k)
      {
        local_mean_estimate[NumberClasses/2 + k] = local_mu + (k+1)*local_sigma/2;
        local_mean_estimate[NumberClasses/2 - 1 - k] = local_mu - (k+1)*local_sigma/2;
      }
    }
    else
    {
      //shared_mean_estimate[NumberClasses/2] = local_mu;
      local_mean_estimate[NumberClasses/2] = local_mu;
      for (int k = 0; k < NumberClasses/2; ++k)
      {
        local_mean_estimate[NumberClasses/2 + 1 + k] = local_mu + (k+1)*local_sigma/2;
        local_mean_estimate[NumberClasses/2 - 1 - k] = local_mu - (k+1)*local_sigma/2;
      }
    }
    // initialize variance
    for (int k = 0; k < NumberClasses; ++k)
      local_variance[k] = 20;
      //shared_variance[k] = 20;
  }

  double Gamma = m_Gamma;

  // allocate needed data
  double *local_probs[MAX_CLASSES];
  double *local_yk[MAX_CLASSES];
  for (int k = 0; k < NumberClasses; ++k)
  {
    local_probs[k] = new double[extent_ijdim];
    local_yk[k] = new double[extent_ijdim];
  }


  int local_class_count[MAX_CLASSES];
  double gamma[MAX_CLASSES];
  for (int i = 0; i < NumberClasses; ++i)
  {
    local_class_count[i] = 0;
    gamma[i] = Gamma;
  }

  // initialize classifications of each pixel randomly with a uniform distribution
  for (size_t i = 0; i < ijdim; ++i)
  {
    if (outputData[i] != 0)
    {
      outputData[i] = static_cast<S>(random.next(0, 1)*(NumberClasses + 1));
    }
  }

  uint32_t EMIterations = m_EmIterations;
  double Beta = m_Beta;

  // create beta ramp for simulated annealing
  // ramps beta continuously from min to 10*min
  double* bt = new double[EMIterations];
  for (uint32_t i = 0; i < EMIterations; ++i)
  {
    bt[i] = Beta;
    if (m_UseSimulatedAnnealing == true)
    {
      bt[i] = bt[i] + ::pow(i/(EMIterations-1.0), 8) * (10.0*Beta - Beta);
    }
  }

  // random access for mpm
 // int *index = new int[extent_ijdim];
 // int temp_index = 0;
  m_TotalIterations = m_EmIterations * m_MpmIterations;
  m_CurrentIteration = 0;
  // perform EM
  for (uint32_t k = 0; k < EMIterations; ++k)
  {
    if (m_Cancel == true) { break; }
    double local_N[MAX_CLASSES];

    // perform MPM
    mpm<T>( imageData, xt, local_probs, local_yk, bt[k], gamma, local_mean_estimate, local_variance, idim, jdim, random);
    // reset model parameters to zero
    if (m_Cancel == true) { break; }
    for (int l = 0; l < NumberClasses; ++l)
    {
      local_mean_estimate[l] = local_variance[l] = local_N[l] = 0.0;
    }
 //   millis = MXA::getMilliSeconds();
    // update mean estimates
    for (int l = 0; l < NumberClasses; ++l)
    {
      int temp_index = 0;
      for (size_t i = 0; i < ijdim; ++i)
      {
        if (imageData[i] != 0)
        {
          local_N[l] += local_probs[l][temp_index];
          local_mean_estimate[l] += imageData[i] * local_probs[l][temp_index];
          ++temp_index;
        }
      }
    }

    // combine results
    for (int l = 0; l < NumberClasses; ++l) {
      if (local_N[l] != 0) {
        local_mean_estimate[l] = local_mean_estimate[l] / local_N[l];
//        std::cout << "local_mean_estimate[" << l << "]: " << local_mean_estimate[l] << std::endl;
//        std::cout << "local_N[" << l << "]: " << local_N[l] << std::endl;
      }
    }

    // update variance estimates
    for (int l = 0; l < NumberClasses; ++l)
    {
      int temp_index = 0;

      for (size_t i = 0; i < ijdim; ++i)
      {
        if (imageData[i] != 0)
        {
          local_variance[l] += (imageData[i] - local_mean_estimate[l]) * (imageData[i] - local_mean_estimate[l])
              * local_probs[l][temp_index];
          ++temp_index;
        }
      }
    }

    // combine results
    for (int l = 0; l < NumberClasses; ++l)
    {
      if (local_N[l] != 0) {
        local_variance[l] = local_variance[l] / local_N[l];
  //      std::cout << "local_variance[" << l << "]: " << local_variance[l] << std::endl;
      }
    }
//    millis = MXA::getMilliSeconds() - millis;
//    std::cout << "Time: " << millis << std::endl;
    // monitor estimates of mean and variance
    if (m_Debug)
    {
      if (EMIterations < 10 || (k + 1) % (EMIterations / 10) == 0)
      {
        for (int l = 0; l < NumberClasses - 1; l++) {
          printf("%.3f %.3f ", local_mean_estimate[l], local_variance[l]);
        }
        printf("%.3f %.3f\n", local_mean_estimate[NumberClasses - 1], local_variance[NumberClasses - 1]);
      }
    }

  }

  if (m_Cancel == false) {
    // scale range of classes to fit pixel depth
    for (size_t i = 0; i < ijdim; ++i)
    {
      if (outputData[i] != 0)
      {
        outputData[i] = static_cast<S>(outputData[i] * 255 / (NumberClasses - 1 ) );
        //outputData[i] = 175 + outputData[i];
      }
    }
  }
  //Clean up all the memory that got allocated:
  for (int k = 0; k < NumberClasses; ++k)
  {
    delete[] local_probs[k];
    delete[] local_yk[k];
  }
  delete[] bt;
}








#endif /* _EMMPM_TASK_H_ */
