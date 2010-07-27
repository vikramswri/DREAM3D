///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
// This code was partly written under US Air Force Contract FA8650-07-D-5800
//
///////////////////////////////////////////////////////////////////////////////
#ifndef QFILECOMPLETER_H_
#define QFILECOMPLETER_H_

#include <QtCore/QObject>
#include <QtGui/QCompleter>

/**
 * @class QFileCompleter QFileCompleter.h QtSupport/QFileCompleter.h
 * @brief Class to extend to complete file paths in QLineEdits
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Jul 26, 2010
 * @version 1.0
 */
class QFileCompleter : public QCompleter
{
  Q_OBJECT;

public:
  /**
   * @brief Contrstructor
   * @param o Parent QObject
   * @param dirs Should the completer ONLY show directories
   * @return
   */
  QFileCompleter(QObject* o, bool dirs);

  /**
   * @brief Returns a file path from the given QModelIndex
   * @param idx The QModelIndex to look up a path
   * @return
   */
  virtual QString pathFromIndex(const QModelIndex& idx) const;
};

#endif /* QFILECOMPLETER_H_ */
