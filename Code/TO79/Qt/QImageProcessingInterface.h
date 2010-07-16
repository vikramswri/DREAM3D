///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef QIMAGEPROCESSINGINTERFACE_H_
#define QIMAGEPROCESSINGINTERFACE_H_



#include <QtPlugin>

class QImageProcessingInterface
{
  public:
    virtual ~QImageProcessingInterface(){};

    virtual void setName (const QString &str) = 0;
};


Q_DECLARE_INTERFACE(QImageProcessingInterface,
                    "net.BlueQuartz.QImageProcessingInterface/1.0")

#endif /* QIMAGEPROCESSINGINTERFACE_H_ */
