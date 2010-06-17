///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////
#include "AboutBox.h"
#include <EMMPM/Common/EMMPMVersion.h>

#include <iostream>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AboutBox::AboutBox(QWidget *parent) :
  QDialog(parent)
{
  this->setupUi(this);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AboutBox::~AboutBox()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AboutBox::setApplicationName(QString applicationName)
{
  QString title("About ");
  title.append(applicationName);
  setWindowTitle(title);

  appName->setText(applicationName);
  QString version("");
  version.append(EMMPM::Version::PackageComplete.c_str());
  appVersion->setText( version );
}


