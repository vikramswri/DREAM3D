///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef R3DABOUTBOX_H_
#define R3DABOUTBOX_H_

#include <QtGui/QDialog>

//-- UIC generated Header
#include <ui_AboutBox.h>


/**
 * @class AboutBox AboutBox.h R3D/Common/Qt/AboutBox.h
 * @brief This will display a simple dialog box that acts like an "About Box" for
 * showing some basic information about the application being used.
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Jul 14, 2009
 * @version 1.0
 */
class AboutBox: public QDialog, private Ui::AboutBox
{
  Q_OBJECT
    ;

  public:
    AboutBox(QWidget *parent = 0);
    virtual ~AboutBox();

    void setApplicationName( QString appName);

  private:
    AboutBox(const AboutBox&); // Copy Constructor Not Implemented
    void operator=(const AboutBox&); // Operator '=' Not Implemented


};

#endif /* R3DABOUTBOX_H_ */
