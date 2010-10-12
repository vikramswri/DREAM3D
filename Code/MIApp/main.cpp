/* --------------------------------------------------------------------------*
* This source code has been cleared for public release by the                *
* US Air Force 88th Air Base Wing Public Affairs Office under                *
* case number 88ABW-2010-4857 on Sept. 7, 2010.                              *
* -------------------------------------------------------------------------- */
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, 2010 Michael A. Jackson for BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
//  This code was written under United States Air Force Contract number
//                           F33615-03-C-5220
//
///////////////////////////////////////////////////////////////////////////////

#include <MIApp/QMIApp.h>
#include <QtSupport/QRecentFileList.h>
#include <QApplication>

int main(int argc, char **argv)
{
  QApplication app(argc, argv);
  QCoreApplication::setOrganizationName("BlueQuartz Software");
  QCoreApplication::setOrganizationDomain("bluequartz.net");
  QCoreApplication::setApplicationName("QGuiAlignment");

#if defined( Q_WS_MAC )
  //Needed for typical Mac program behavior.
  app.setQuitOnLastWindowClosed( true );
#endif //APPLE

  #if defined (Q_OS_MAC)
  QSettings prefs(QSettings::NativeFormat, QSettings::UserScope, "bluequartz.net", "QMIApp");
#else
  QSettings prefs(QSettings::IniFormat, QSettings::UserScope, "bluequartz.net", "QMIApp");
#endif
  QRecentFileList::instance()->readList(prefs);

  QMIApp *viewer = new QMIApp;
  viewer->show();
  viewer->raise();
  viewer->activateWindow();
  int app_return = app.exec();

  QRecentFileList::instance()->writeList(prefs);

  return app_return;
}
