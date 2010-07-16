///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "EMMPMPlugin.h"
#include "EMMPMInputUI.h"

Q_EXPORT_PLUGIN2(EMMPMPlugin, EMMPMPlugin);

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EMMPMPlugin::EMMPMPlugin() :
m_InputWidget(NULL)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EMMPMPlugin::~EMMPMPlugin()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString EMMPMPlugin::getPluginName()
{
  return QString("EM/MPM");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QWidget* EMMPMPlugin::getInputWidget(QWidget* parent)
{
  if (NULL == m_InputWidget)
  {
    m_InputWidget = new EMMPMInputUI(parent);
  }
  m_InputWidget->setParent(parent);
  return m_InputWidget;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int EMMPMPlugin::startProcessing(QObject* caller)
{
  return m_InputWidget->processInputs(caller);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString EMMPMPlugin::getInputImage()
{
  return m_InputWidget->getCurrentImageFile();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString EMMPMPlugin::getProcessedImage()
{
  return m_InputWidget->getCurrentProcessedFile();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMMPMPlugin::writeSettings(QSettings* prefs)
{
  m_InputWidget->writeSettings(prefs);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMMPMPlugin::readSettings(QSettings* prefs)
{
  m_InputWidget->readSettings(prefs);
}
