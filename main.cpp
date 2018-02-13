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

#include "provider.h"
#include "renderer.h"

#include <unistd.h>
#include <QtCore/QCoreApplication>

class TestProvider : public Provider {
  public:
    virtual ~TestProvider() {}

    virtual QString id() const { return QStringLiteral("test"); }
    virtual QVector<Fact> render() const {
      return {
        Fact { Fact::Network, Fact::Passive, QStringLiteral("1.2.3.4") },
        Fact { Fact::Battery, Fact::Positive, QStringLiteral("100%") },
        Fact { Fact::Network, Fact::Passive, QStringLiteral("5.6.7.8") },
      };
    }
};

int main(int argc, char** argv) {
  QCoreApplication app(argc, argv);
  Renderer r({new TestProvider}, &app);

  for (;;) {
    r.render();

    sleep(1);
  }
}
