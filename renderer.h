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

#ifndef QI3STATUS_RENDERER_H
#define QI3STATUS_RENDERER_H

class Provider;
#include <QtCore/QObject>
#include <QtCore/QVector>

class Renderer : public QObject {
  Q_OBJECT
  public:
    Renderer(const QVector<Provider*>& providers, QObject* parent = Q_NULLPTR);
    virtual ~Renderer();
  public Q_SLOTS:
    void render();
  private:
    QVector<Provider*> m_providers;
};

#endif // QI3STATUS_RENDERER_H
