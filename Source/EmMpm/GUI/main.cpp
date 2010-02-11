///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2010, mjackson
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
//  This code was written under United States Air Force Contract number
//                           F33615-03-C-5220
//
///////////////////////////////////////////////////////////////////////////////

#include "QEmMpm.h"

//-- Qt Headers
#include <QtCore/QString>
#include <QtCore/QDir>
#include <QtCore/QPluginLoader>
#include <QtGui/QApplication>


/**
 * @brief The Main entry point for the application
 */
int main (int argc, char *argv[])
{
  QApplication app(argc, argv);
  QCoreApplication::setOrganizationName("BlueQuartz Software");
  QCoreApplication::setOrganizationDomain("bluequartz.net");
  QCoreApplication::setApplicationName("QEmMpm");


  QString plugin_path;
  QStringList filters;
  filters << "*.dll" << "*.dylib";

#if 1
#if defined (Q_OS_MAC)
    //Look in the Application Package "Application.app/Contents/Plugins
    QDir appDir (QCoreApplication::applicationDirPath());
    appDir.cdUp();


    {
    plugin_path = appDir.absolutePath() + QDir::separator() + "Libraries";
    std::cout << "plugin_path: " << plugin_path.toStdString() << std::endl;
    QDir cPath(plugin_path);
    QStringList plugins = cPath.entryList(filters);
    for (int i = 0; i < plugins.size(); ++i)
    {
      std::cout << "Trying to load: " << plugins.at(i).toStdString() << std::endl;
      QPluginLoader qplugin(plugin_path + QDir::separator() + plugins.at(i));
       if(qplugin.load())
       {
         std::cout << "Plugin: " << plugins.at(i).toStdString() << " Loaded." << std::endl;
       }
       else
       {
       std::cout << "Plugin did NOT load: Error was " << qplugin.errorString().toStdString() << std::endl;
       }
    }
    }

    {
    plugin_path = appDir.absolutePath() + QDir::separator() + "Plugins";
    std::cout << "plugin_path: " << plugin_path.toStdString() << std::endl;
    QDir cPath(plugin_path);
    QStringList plugins = cPath.entryList(filters);
    for (int i = 0; i < plugins.size(); ++i)
    {
      QPluginLoader qplugin(plugins.at(i));
         if(qplugin.load())
         {
           std::cout << "Plugin: " << plugins.at(i).toStdString() << " Loaded." << std::endl;
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


#endif


#if defined( Q_WS_MAC )
  //Needed for typical Mac program behavior.
  app.setQuitOnLastWindowClosed( true );
#endif //APPLE
  QEmMpm *viewer = new QEmMpm;
  viewer->show();
  int app_return = app.exec();

  // When the GUI exits we will be at this point
  // Cleanup
  //delete viewer;

  return app_return;
}



