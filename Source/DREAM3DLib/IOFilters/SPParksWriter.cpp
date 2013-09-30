/* ============================================================================
 * Copyright (c) 2011 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
 * Neither the name of Michael A. Groeber, Michael A. Jackson, the US Air Force,
 * BlueQuartz Software nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior written
 * permission.
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
 *
 *  This code was written under United States Air Force Contract number
 *                           FA8650-07-D-5800
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "SPParksWriter.h"

#include <fstream>
#include <QtCore/QDateTime>
//#include <QtCore/QString>

#include <sstream>

#include <QtCore/QtDebug>
#include <QtCore/QDateTime>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QFile>

#include "DREAM3DLib/Utilities/TimeUtilities.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SPParksWriter::SPParksWriter() :
FileWriter(),
m_DataContainerName(DREAM3D::HDF5::VolumeDataContainerName),
m_GrainIdsArrayName(DREAM3D::CellData::GrainIds),
m_GrainIds(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SPParksWriter::~SPParksWriter()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SPParksWriter::setupFilterParameters()
{
  QVector<FilterParameter::Pointer> parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Output File");
    option->setPropertyName("OutputFile");
    option->setWidgetType(FilterParameter::OutputFileWidget);
    option->setFileExtension("*.spparks");
    option->setFileType("SPParks Sites File");
    option->setValueType("string");
    parameters.push_back(option);
  }
  setFilterParameters(parameters);
}
// -----------------------------------------------------------------------------
void SPParksWriter::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
/* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setOutputFile( reader->readValue( "OutputFile", getOutputFile() ) );
/* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SPParksWriter::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("OutputFile", getOutputFile() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SPParksWriter::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  GET_PREREQ_DATA(m, DREAM3D, CellData, GrainIds, -300, int32_t, Int32ArrayType, voxels, 1)
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SPParksWriter::preflight()
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }

  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SPParksWriter::writeHeader()
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  if (NULL == m)
  {
    QString ss = QObject::tr("DataContainer Pointer was NULL and Must be valid.%1(%2)").arg(__FILE__).arg(__LINE__);
    addErrorMessage(getHumanLabel(), ss, -2);
    setErrorCondition(-1);
    return -1;
  }

  int64_t totalPoints = m->getTotalPoints();
  dataCheck(false, totalPoints, 1, 1);
  if (getErrorCondition() < 0)
  {
    return -40;
  }

  size_t udims[3] = {0,0,0};
  m->getDimensions(udims);
#if (CMP_SIZEOF_SIZE_T == 4)
  typedef int32_t DimType;
#else
  typedef int64_t DimType;
#endif
  DimType dims[3] = {
    static_cast<DimType>(udims[0]),
    static_cast<DimType>(udims[1]),
    static_cast<DimType>(udims[2]),
  };
  int64_t totalpoints = dims[0] * dims[1] * dims[2];


  std::ofstream outfile;
  outfile.open(getOutputFile().toLatin1().data(), std::ios_base::binary);
  if(!outfile)
  {
    qDebug() << "Failed to open: " << getOutputFile() ;
    return -1;
  }

  outfile << "-" << "\n";
  outfile << "3 dimension" << "\n";
  outfile << totalpoints << " sites" << "\n";
  outfile << "26 max neighbors" << "\n";
  outfile << "0 " << udims[0] << " xlo xhi" << "\n";
  outfile << "0 " << udims[1] << " ylo yhi" << "\n";
  outfile << "0 " << udims[2] << " zlo zhi" << "\n";
  outfile << "\n";
  outfile << "Values" << "\n";
  outfile << "\n";
  outfile.close();
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SPParksWriter::writeFile()
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  int64_t totalPoints = m->getTotalPoints();
  dataCheck(false, totalPoints, 1, 1);
  if (getErrorCondition() < 0)
  {
    return -40;
  }

  size_t udims[3] = {0,0,0};
  m->getDimensions(udims);
#if (CMP_SIZEOF_SIZE_T == 4)
  typedef int32_t DimType;
#else
  typedef int64_t DimType;
#endif
  DimType dims[3] = {
    static_cast<DimType>(udims[0]),
    static_cast<DimType>(udims[1]),
    static_cast<DimType>(udims[2]),
  };
  int64_t totalpoints = dims[0] * dims[1] * dims[2];


  std::ofstream outfile;
  outfile.open(getOutputFile().toLatin1().data(), std::ios_base::binary | std::ios_base::app);
  if(!outfile)
  {
    qDebug() << "Failed to open: " << getOutputFile() ;
    return -1;
  }


  uint64_t millis = QDateTime::currentMSecsSinceEpoch();
  uint64_t currentMillis = millis;
  uint64_t startMillis = millis;
  uint64_t estimatedTime = 0;
  float timeDiff = 0.0f;


  int increment = totalpoints * .01;
  QString buf;
  QTextStream ss(&buf);
  // Buffer the output with 4096 Bytes which is typically the size of a "Block" on a
  // modern Hard Drive. This should speed up the writes considerably
  char buffer[4096];
  outfile.rdbuf()->pubsetbuf(buffer, 4096);
  for (int k = 0; k < totalpoints; k++)
  {
    if (totalpoints % increment == 0)
    {
      currentMillis = QDateTime::currentMSecsSinceEpoch();
      if (currentMillis - millis > 1000)
      {
        ss.reset();
        ss << static_cast<int>((float)(k)/(float)(totalpoints) * 100)<< " % Completed ";
        timeDiff = ((float)k / (float)(currentMillis - startMillis));
        estimatedTime = (float)(totalpoints - k) / timeDiff;
        ss << " Est. Time Remain: " << DREAM3D::convertMillisToHrsMinSecs(estimatedTime);
        notifyStatusMessage( buf );
        millis = QDateTime::currentMSecsSinceEpoch();
      }
    }
    outfile << k + 1 << " " << m_GrainIds[k] << "\n";
  }
  outfile.close();

  // If there is an error set this to something negative and also set a message
  notifyStatusMessage("Writing Ph File Complete");
  return 0;
}

