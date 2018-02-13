/*******************************************************************************
*
* Copyright 2018 Stefan Majewsky <majewsky@gmx.net>
*
* This program is free software: you can redistribute it and/or modify it under
* the terms of the GNU General Public License as published by the Free Software
* Foundation, either version 3 of the License, or (at your option) any later
* version.
*
* This program is distributed in the hope that it will be useful, but WITHOUT ANY
* WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
* A PARTICULAR PURPOSE. See the GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License along with
* this program. If not, see <http://www.gnu.org/licenses/>.
*
*******************************************************************************/

#ifndef QI3STATUS_H
#define QI3STATUS_H

class QJsonArray;
class QObject;
#include <QtCore/QVector>

struct Fact {
  enum Class {
    Network,
    Brightness,
    Volume,
    Battery,
    MissingService,
  };
  enum Priority {
    Positive,
    Passive,
    Danger,
  };

  Class class_;
  Priority priority;
  QString text;
};

class Provider {
  public:
    virtual ~Provider() {}

    virtual QString id() const = 0;
    virtual QVector<Fact> render() const = 0;
    virtual void connectUpdatesTo(QObject* object, const char* slot) = 0;
    virtual void execCommand(const QStringList& args) { Q_UNUSED(args) }
};

Provider* buildUpowerProvider(QObject* parent = Q_NULLPTR);

#endif // QI3STATUS_H
