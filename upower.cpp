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

#include <QtCore/QObject>
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusReply>

class UpowerProvider : public QObject, public Provider {
  Q_OBJECT
  public:
    UpowerProvider(QObject* parent = Q_NULLPTR);
    virtual QString id() const { return "upower"; }
    virtual void connectUpdatesTo(QObject* object, const char* slot);
    virtual QVector<Fact> render() const;
  private:
    QDBusInterface* m_interface;
    QDBusInterface* m_deviceInterface;
};

Provider* buildUpowerProvider(QObject* parent) {
  return new UpowerProvider(parent);
}

UpowerProvider::UpowerProvider(QObject* parent)
  : QObject(parent)
  , m_interface(Q_NULLPTR)
  , m_deviceInterface(Q_NULLPTR)
{
  m_interface = new QDBusInterface(
    "org.freedesktop.UPower",
    "/org/freedesktop/UPower",
    "org.freedesktop.UPower",
    QDBusConnection::systemBus()
  );
  if (!m_interface->isValid()) {
    return;
  }

  QDBusReply<QString> reply = m_interface->call("GetDisplayDevice");
  const QString displayDevicePath = reply.isValid()
    ? reply.value()
    : "/org/freedesktop/UPower/devices/DisplayDevice";

  m_deviceInterface = new QDBusInterface(
    "org.freedesktop.UPower",
    displayDevicePath,
    "org.freedesktop.UPower.Device",
    QDBusConnection::systemBus()
  );
}

void UpowerProvider::connectUpdatesTo(QObject* object, const char* slot)
{
  //TODO: listen to property changes; query properties only when changed
}

QVector<Fact> UpowerProvider::render() const
{
  if (!m_interface->isValid() || !m_deviceInterface->isValid()) {
    return QVector<Fact>({
      Fact { Fact::MissingService, Fact::Danger, QStringLiteral("upower") },
    });
  }

  const bool onBattery = qvariant_cast<bool>(m_interface->property("OnBattery"));
  const double percent = qvariant_cast<double>(m_deviceInterface->property("Percentage"));

  int percentDisplay = int(percent);
  if (percentDisplay < 0) {
    percentDisplay = 0;
  }
  if (percentDisplay > 100) {
    percentDisplay = 100;
  }
  const QString text = QStringLiteral("%1%").arg(percentDisplay);

  Fact::Priority prio = !onBattery   ? Fact::Positive
                      : percent < 20 ? Fact::Danger
                      :                Fact::Passive;

  return QVector<Fact>({
    Fact { Fact::Battery, prio, text }
  });
}

#include "upower.moc"
