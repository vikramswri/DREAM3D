///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef R3DApplicationAboutBoxDialog_H_
#define R3DApplicationAboutBoxDialog_H_

#include <QtGui/QDialog>

//-- UIC generated Header
#include <ui_ApplicationAboutBoxDialog.h>


/**
 * @class ApplicationAboutBoxDialog ApplicationAboutBoxDialog.h R3D/Common/Qt/ApplicationAboutBoxDialog.h
 * @brief This will display a simple dialog box that acts like an "About Box" for
 * showing some basic information about the application being used.
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Jul 14, 2009
 * @version 1.0
 */
class ApplicationAboutBoxDialog: public QDialog, private Ui::ApplicationAboutBoxDialog
{
  Q_OBJECT
    ;

  public:
    ApplicationAboutBoxDialog(QWidget *parent = 0);
    virtual ~ApplicationAboutBoxDialog();

    /**
     * @brief Sets information about the application
     * @param appName
     * @param appVersion
     */
    void setApplicationInfo( QString appName, QString appVersion);

    void setApplicationHelpText(const QString text);

  private:
    ApplicationAboutBoxDialog(const ApplicationAboutBoxDialog&); // Copy Constructor Not Implemented
    void operator=(const ApplicationAboutBoxDialog&); // Operator '=' Not Implemented


};

#endif /* R3DApplicationAboutBoxDialog_H_ */
