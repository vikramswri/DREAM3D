///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "CrossCorrelationPlugin.h"
#include "CrossCorrelationInputUI.h"

Q_EXPORT_PLUGIN2(CrossCorrelationPlugin, CrossCorrelationPlugin);

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CrossCorrelationPlugin::CrossCorrelationPlugin()
{
  m_InputWidget = new CrossCorrelationInputUI(NULL);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CrossCorrelationPlugin::~CrossCorrelationPlugin()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString CrossCorrelationPlugin::getPluginName()
{
  return QString("Cross Correlation");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QWidget* CrossCorrelationPlugin::getInputWidget(QWidget* parent)
{
  m_InputWidget->setParent(parent);
  return m_InputWidget;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int CrossCorrelationPlugin::startProcessing(QObject* caller)
{
  return m_InputWidget->processInputs(caller);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString CrossCorrelationPlugin::getInputImage()
{
  return m_InputWidget->getCurrentImageFile();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString CrossCorrelationPlugin::getProcessedImage()
{
  return m_InputWidget->getCurrentProcessedFile();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CrossCorrelationPlugin::writeSettings(QSettings* prefs)
{
  m_InputWidget->writeSettings(prefs);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CrossCorrelationPlugin::readSettings(QSettings* prefs)
{
  m_InputWidget->readSettings(prefs);
}
