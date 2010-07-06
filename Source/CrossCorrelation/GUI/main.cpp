///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, 2010 Michael A. Jackson for BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "QCrossCorrelation.h"

//-- Qt Headers
#include <QtGui/QApplication>

/**
 * @brief The Main entry point for the application
 */
int main (int argc, char *argv[])
{
  QApplication app(argc, argv);
  QCoreApplication::setOrganizationName("BlueQuartz Software");
  QCoreApplication::setOrganizationDomain("bluequartz.net");
  QCoreApplication::setApplicationName("QCrossCorrelation");
#if defined( Q_WS_MAC )
  //Needed for typical Mac program behavior.
  app.setQuitOnLastWindowClosed( true );
#endif //APPLE
  QCrossCorrelation *viewer = new QCrossCorrelation;
  viewer->show();
  int app_return = app.exec();
  return app_return;
}
