/* ============================================================================
 * Copyright (c) 2017-2019 BlueQuartz Software, LLC
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice, this
 * list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * Neither the names of any of the BlueQuartz Software contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

// C Includes
#include <assert.h>
#include <stdlib.h>

// C++ Includes
#include <iostream>

// Qt Includes
#include <QtCore/QCommandLineOption>
#include <QtCore/QCommandLineParser>
#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QSettings>
#include <QtCore/QString>
#include <QtCore/QtDebug>

#include "QtWebApp/httpserver/ServerSettings.h"
#include "QtWebApp/httpserver/httplistener.h"
#include "QtWebApp/httpserver/httpsessionstore.h"
#include "QtWebApp/logging/filelogger.h"

// DREAM3DLib includes
#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/H5FilterParametersReader.h"
#include "SIMPLib/FilterParameters/JsonFilterParametersReader.h"
#include "SIMPLib/Filtering/FilterFactory.hpp"
#include "SIMPLib/Filtering/FilterManager.h"
#include "SIMPLib/Filtering/FilterPipeline.h"
#include "SIMPLib/Filtering/QMetaObjectUtilities.h"
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"
#include "SIMPLib/REST/SIMPLRequestMapper.h"
#include "SIMPLib/REST/V1Controllers/SIMPLStaticFileController.h"
#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/SIMPLibVersion.h"

// -----------------------------------------------------------------------------
// Search the configuration file
// -----------------------------------------------------------------------------
QString searchConfigFile()
{
  QString binDir = QCoreApplication::applicationDirPath();
  QString appName = QCoreApplication::applicationName();
  QString fileName(appName + ".ini");

  QStringList searchList;
  searchList.append(binDir);

  foreach(QString dir, searchList)
  {
    QFile file(dir + "/" + fileName);
    if(file.exists())
    {
      // found
      fileName = QDir(file.fileName()).canonicalPath();
      qDebug("Using config file %s", qPrintable(fileName));
      return fileName;
    }
  }

  // not found
  foreach(QString dir, searchList)
  {
    qWarning("%s/%s not found", qPrintable(dir), qPrintable(fileName));
  }
  return QString();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int main(int argc, char* argv[])
{
  QCoreApplication app(argc, argv);

  app.setApplicationName("RESTServer");
  app.setOrganizationName("BlueQuartz Software");

  //
  // Register all the filters including trying to load those from Plugins
  FilterManager* fm = FilterManager::Instance();
  SIMPLibPluginLoader::LoadPluginFilters(fm);
  //
  QMetaObjectUtilities::RegisterMetaTypes();

  // Find the configuration file
  QString configFileName = searchConfigFile();
  if(configFileName.isEmpty())
  {
    qDebug() << "Input configuration file was not found. Server cannot continue.";
    return 1;
  }
  // Configure logging into a file
  /*
    QSettings* logSettings=new QSettings(configFileName,QSettings::IniFormat,&app);
    logSettings->beginGroup("logging");
    FileLogger* logger=new FileLogger(logSettings,10000,&app);
    logger->installMsgHandler();
    */

  // Configure template loader and cache
  //    QSettings* templateSettings=new QSettings(configFileName,QSettings::IniFormat,&app);
  //    templateSettings->beginGroup("templates");
  //    templateCache=new TemplateCache(templateSettings,&app);

  // Configure session store

  QSettings config(configFileName, QSettings::IniFormat, &app);
  ServerSettings serverSettings(config);

  HttpSessionStore* sessionStore = HttpSessionStore::CreateInstance(&serverSettings, &app);

  // Configure static file controller
  SIMPLStaticFileController::CreateInstance(&serverSettings, &app);

  // Configure and start the TCP listener
  QSharedPointer<HttpListener> httpListener = QSharedPointer<HttpListener>(new HttpListener(&serverSettings, new SIMPLRequestMapper(&app), &app));

  qWarning() << "Application has started";

  app.exec();

  qWarning() << "Application has stopped";

  ////////
  return 0;
}
