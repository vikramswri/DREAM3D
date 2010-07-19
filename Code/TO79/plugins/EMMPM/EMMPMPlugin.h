///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef EMMPMPLUGIN_H_
#define EMMPMPLUGIN_H_

#include <QtCore/QObject>
#include <QtCore/QSettings>
#include "TO79/plugins/QImageProcessingInterface.h"

class EMMPMInputUI;

class EMMPMPlugin : public QObject, public QImageProcessingInterface
{
    Q_OBJECT;
    Q_INTERFACES(QImageProcessingInterface )

  public:
    EMMPMPlugin();
    virtual ~EMMPMPlugin();

    QString getPluginName();

    virtual QWidget* getInputWidget(QWidget* parent);

    virtual int startProcessing(QObject* caller);

    virtual QString getInputImage();
    virtual QString getProcessedImage();

    virtual void writeSettings(QSettings* prefs);
    virtual void readSettings(QSettings* prefs);

  protected:
    EMMPMInputUI* m_InputWidget;

  private:
    EMMPMPlugin(const EMMPMPlugin&); // Copy Constructor Not Implemented
    void operator=(const EMMPMPlugin&); // Operator '=' Not Implemented
};

#endif /* EMMPMPLUGIN_H_ */
