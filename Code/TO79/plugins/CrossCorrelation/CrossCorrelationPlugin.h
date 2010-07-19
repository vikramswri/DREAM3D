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
/*
 *
 */
class CrossCorrelationPlugin : public QObject, public QImageProcessingInterface
{
    Q_OBJECT;
    Q_INTERFACES(QImageProcessingInterface )

  public:
    CrossCorrelationPlugin();
    virtual ~CrossCorrelationPlugin();

    QString getPluginName();
    virtual QWidget* getInputWidget(QWidget* parent);


    virtual int startProcessing(QObject* caller);

    virtual QString getInputImage();
    virtual QString getProcessedImage();

    virtual void writeSettings(QSettings* prefs);
    virtual void readSettings(QSettings* prefs);

  protected:
    CrossCorrelationInputUI* m_InputWidget;

  private:
    CrossCorrelationPlugin(const CrossCorrelationPlugin&); // Copy Constructor Not Implemented
    void operator=(const CrossCorrelationPlugin&); // Operator '=' Not Implemented
};

#endif /* CROSSCORRELATIONPLUGIN_H_ */
