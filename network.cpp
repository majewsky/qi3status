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

#include <algorithm>
#include <QtNetwork/QNetworkInterface>

class NetworkProvider : public Provider {
  public:
    virtual QString id() const { return "network"; }
    virtual void connectUpdatesTo(QObject*, const char*) { /*unused*/ }
    virtual QVector<Fact> render() const;
};

Provider* buildNetworkProvider() {
  return new NetworkProvider;
}

QVector<Fact> NetworkProvider::render() const {
  QVector<Fact> result;

  for (const auto iface: QNetworkInterface::allInterfaces()) {
    //filter local bridges, loopbacks, interfaces that are down
    if (iface.name() == "docker0") {
      continue;
    }
    const auto flags = iface.flags();
    if (flags & QNetworkInterface::IsLoopBack) {
      continue;
    }
    if (!(flags & QNetworkInterface::IsUp)) {
      continue;
    }

    for (const auto entry: iface.addressEntries()) {
      const auto ip = entry.ip();

      //filter non-global IPs
      if (!ip.scopeId().isEmpty()) {
        continue;
      }
      //distinguish v4 and v6 IPs by color
      const bool isIPv6 = ip.protocol() == QAbstractSocket::IPv6Protocol;

      result.append(Fact { Fact::Network, isIPv6 ? Fact::Muted : Fact::Passive, ip.toString() });
    }
  }

  std::sort(result.begin(), result.end(), [](const Fact& x, const Fact& y) {
    return x.text < y.text;
  });
  return result;
}
