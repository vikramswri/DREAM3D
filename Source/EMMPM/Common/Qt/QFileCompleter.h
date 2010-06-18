///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
// This code was partly written under US Air Force Contract FA8650-07-D-5800
//
///////////////////////////////////////////////////////////////////////////////
#ifndef R3DFILECOMPLETER_H_
#define R3DFILECOMPLETER_H_

#include <QtCore/QObject>
#include <QtGui/QCompleter>

class QFileCompleter : public QCompleter
{
  Q_OBJECT
public:
  QFileCompleter(QObject* o, bool dirs);
  virtual QString pathFromIndex(const QModelIndex& idx) const;
};

#endif /* R3DFILECOMPLETER_H_ */
