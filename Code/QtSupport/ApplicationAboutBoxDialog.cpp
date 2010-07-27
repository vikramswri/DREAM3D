///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////
#include "ApplicationAboutBoxDialog.h"
#include "TO79/Common/TO79Version.h"

#include <iostream>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ApplicationAboutBoxDialog::ApplicationAboutBoxDialog(QWidget *parent) :
  QDialog(parent)
{
  this->setupUi(this);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ApplicationAboutBoxDialog::~ApplicationAboutBoxDialog()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ApplicationAboutBoxDialog::setApplicationInfo(QString applicationName, QString version)
{
  QString title("About ");
  title.append(applicationName);
  setWindowTitle(title);
  appName->setText(applicationName);
  appVersion->setText( version );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ApplicationAboutBoxDialog::setApplicationHelpText(const QString text)
{
  this->appHelpText->setText(text);
}
