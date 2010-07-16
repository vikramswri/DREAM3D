///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2010, mjackson
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
//  This code was written under United States Air Force Contract number
//                           FA8650-07-D-5800
//
///////////////////////////////////////////////////////////////////////////////

#include "QEmMpm.h"

//-- Qt Headers
#include <QtCore/QString>
#include <QtCore/QDir>
#include <QtCore/QPluginLoader>
#include <QtGui/QApplication>
#include <QtDebug>

/**
 * @brief The Main entry point for the application
 */
int main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  QCoreApplication::setOrganizationName("BlueQuartz Software");
  QCoreApplication::setOrganizationDomain("bluequartz.net");
  QCoreApplication::setApplicationName("QEmMpm");

  QString plugin_path;
  QStringList filters;
  filters << "*.dll" << "*.dylib";


#if defined (Q_OS_MAC)
  //Look in the Application Package "Application.app/Contents/Plugins
  QDir appDir(QCoreApplication::applicationDirPath());
  appDir.cdUp();

  {
    plugin_path = appDir.absolutePath() + QDir::separator() + "Libraries";
 //   qWarning() << "plugin_path: " << plugin_path;
    QDir cPath(plugin_path);
    QStringList plugins = cPath.entryList(filters);
    for (int i = 0; i < plugins.size(); ++i)
    {
   //   qWarning() << "Trying to load: " << plugins.at(i);
      QPluginLoader qplugin(plugin_path + QDir::separator() + plugins.at(i));
      if (qplugin.load())
      {
   //     qWarning() << "Plugin: " << plugins.at(i) << " Loaded.";
      }
      else
      {
    //    qWarning() << "Plugin did NOT load: Error was " << qplugin.errorString();
      }
    }
  }

  {
    plugin_path = appDir.absolutePath() + QDir::separator() + "Plugins";
    //  std::cout << "plugin_path: " << plugin_path.toStdString() << std::endl;
    QDir cPath(plugin_path);
    QStringList plugins = cPath.entryList(filters);
    for (int i = 0; i < plugins.size(); ++i)
    {
      QPluginLoader qplugin(plugins.at(i));
      if (qplugin.load())
      {
        qWarning() << "Plugin: " << plugins.at(i) << " Loaded.";
      }
    }
  }

  //Look for a folder called "Plugins" at the same level as Application.app
  appDir.cdUp();
  appDir.cdUp();
  plugin_path += appDir.absolutePath() + QDir::separator() + "Plugins;";
  //Look for a plugins at the same level as Application.app
  plugin_path += appDir.absolutePath() + QDir::separator() + ";";
#else
  plugin_path = QCoreApplication::applicationDirPath() + QDir::separator() + "plugins";
#endif


#if defined( Q_WS_MAC )
  //Needed for typical Mac program behavior.
  app.setQuitOnLastWindowClosed(true);
#endif //APPLE
  QEmMpm *viewer = new QEmMpm;
  viewer->show();
  int app_return = app.exec();

  // When the GUI exits we will be at this point
  // Cleanup
  //delete viewer;

  return app_return;
}

