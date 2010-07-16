///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef QIMAGEPROCESSINGINTERFACE_H_
#define QIMAGEPROCESSINGINTERFACE_H_


#include <QtCore/QSettings>
#include <QtPlugin>

class QImageProcessingInterface
{
  public:
    virtual ~QImageProcessingInterface(){};

    virtual QString getPluginName() = 0;
    virtual QWidget* getInputWidget(QWidget* parent) = 0;
    virtual int startProcessing(QObject* caller) = 0;

    virtual QString getInputImage() = 0;
    virtual QString getProcessedImage() = 0;

    virtual void writeSettings(QSettings* prefs) = 0;
    virtual void readSettings(QSettings* prefs) = 0;
};


Q_DECLARE_INTERFACE(QImageProcessingInterface,
                    "net.BlueQuartz.QImageProcessingInterface/1.0")

#endif /* QIMAGEPROCESSINGINTERFACE_H_ */
