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
#include "TO79/Qt/QImageProcessingInterface.h"

/*
 *
 */
class EMMPMPlugin : public QObject, public QImageProcessingInterface
{
    Q_OBJECT;
    Q_INTERFACES(QImageProcessingInterface )



  public:
    EMMPMPlugin();
    virtual ~EMMPMPlugin();

    void setName(const QString &str);

  private:
    EMMPMPlugin(const EMMPMPlugin&); // Copy Constructor Not Implemented
    void operator=(const EMMPMPlugin&); // Operator '=' Not Implemented
};

#endif /* EMMPMPLUGIN_H_ */
