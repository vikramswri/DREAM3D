///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
// This code was partly written under US Air Force Contract FA8650-07-D-5800
//
///////////////////////////////////////////////////////////////////////////////
#include "QFileCompleter.h"

#include <QtGui/QDirModel>

// -----------------------------------------------------------------------------
// use same QDirModel for all completers
// -----------------------------------------------------------------------------
static QDirModel* fileDirModel()
{
  static QDirModel* m = NULL;
  if (!m)
  {
    m = new QDirModel();
    QStringList nameFilters;
    nameFilters << "*.*";
    m->setNameFilters(nameFilters);
    m->setFilter(QDir::AllDirs | QDir::Drives | QDir::NoDotAndDotDot | QDir::System | QDir::Files);
  }
  return m;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
static QDirModel* pathDirModel()
{
  static QDirModel* m = NULL;
  if (!m)
  {
    m = new QDirModel();
    m->setFilter(QDir::AllDirs | QDir::Drives | QDir::NoDotAndDotDot);
  }
  return m;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QFileCompleter::QFileCompleter(QObject* o, bool dirs) :
  QCompleter(o)
{
  QDirModel* m = dirs ? pathDirModel() : fileDirModel();
  this->setModel(m);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString QFileCompleter::pathFromIndex(const QModelIndex& idx) const
{
  return QDir::fromNativeSeparators(QCompleter::pathFromIndex(idx));
}

