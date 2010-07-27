///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef CROSSCORRELATIONPLUGIN_H_
#define CROSSCORRELATIONPLUGIN_H_

#include <QtCore/QObject>
#include <QtCore/QSettings>
#include "TO79/plugins/QImageProcessingInterface.h"

class CrossCorrelationInputUI;
/**
 * @class CrossCorrelationPlugin CrossCorrelationPlugin.h CrossCorrelation/CrossCorrelationPlugin.h
 * @brief This plugin implements a Cross Correlation algorithm to register or align
 * a pair of images. The output image will be a possibly shifted (in translation only) version
 * of the 'moving' image. See the parent class QImageProcessingInterface for documentation
 * on each of the methods.
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Jul 27, 2010
 * @version 1.0
 */
class CrossCorrelationPlugin : public QObject, public QImageProcessingInterface
{
    Q_OBJECT;
    Q_INTERFACES(QImageProcessingInterface )

  public:
    CrossCorrelationPlugin();
    virtual ~CrossCorrelationPlugin();

    /* ---- Implementation of pure virtual functions from QImageProcessingInterface class -- */
    QString getPluginName();

    virtual QWidget* getInputWidget(QWidget* parent);

    virtual int startProcessing(QObject* caller);

    virtual QString getInputImage();
    virtual QString getProcessedImage();

    virtual void writeSettings(QSettings* prefs);
    virtual void readSettings(QSettings* prefs);
    /* ---- Other class specific methods --- */


  protected:
    CrossCorrelationInputUI* m_InputWidget;

  private:
    CrossCorrelationPlugin(const CrossCorrelationPlugin&); // Copy Constructor Not Implemented
    void operator=(const CrossCorrelationPlugin&); // Operator '=' Not Implemented
};

#endif /* CROSSCORRELATIONPLUGIN_H_ */
