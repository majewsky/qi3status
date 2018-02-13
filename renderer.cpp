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

#include "renderer.h"
#include "provider.h"

#include <algorithm>
#include <stdio.h>
#include <QtCore/QDateTime>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

static const QString keyName                = QStringLiteral("name");
static const QString keyInstance            = QStringLiteral("instance");
static const QString keyFullText            = QStringLiteral("full_text");
static const QString keySeparator           = QStringLiteral("separator");
static const QString keySeparatorBlockWidth = QStringLiteral("separator_block_width");
static const QString keyColor               = QStringLiteral("color");
static const QString keyUrgent              = QStringLiteral("urgent");
static const QString colorHeading  = QStringLiteral("#AAAAAA");
static const QString colorPositive = QStringLiteral("#00AA00");
static const QString colorPassive  = QStringLiteral("#00AAAA");
static const QString colorDanger   = QStringLiteral("#AA0000");
static const QString colorUnknown  = QStringLiteral("#000000");
static const QString colorDate     = QStringLiteral("#AAAAAA");
static const QString colorTime     = QStringLiteral("#FFFFFF");
static const QString headerNetwork    = QStringLiteral("net");
static const QString headerBrightness = QStringLiteral("light");
static const QString headerVolume     = QStringLiteral("vol");
static const QString headerBattery    = QStringLiteral("bat");
static const QString headerMissing    = QStringLiteral("missing");
static const QString headerUnknown    = QStringLiteral("unknown");
static const QString nameClock    = QStringLiteral("clock");
static const QString instanceDate = QStringLiteral("date");
static const QString instanceTime = QStringLiteral("time");

static const QString& labelForFactClass(Fact::Class c) {
  switch (c) {
    case Fact::Network:        return headerNetwork;
    case Fact::Brightness:     return headerBrightness;
    case Fact::Volume:         return headerVolume;
    case Fact::Battery:        return headerBattery;
    case Fact::MissingService: return headerMissing;
    default:                   return headerUnknown;
  }
}

static const QString& colorForFactPriority(Fact::Priority p) {
  switch (p) {
    case Fact::Positive: return colorPositive;
    case Fact::Passive:  return colorPassive;
    case Fact::Danger:   return colorDanger;
    default:             return colorUnknown;
  }
}

Renderer::Renderer(const QVector<Provider*>& providers, QObject* parent)
  : QObject(parent)
  , m_providers(providers)
{
  //initialize i3bar protocol on stdout
  puts("{\"version\":1}\n[");
  //render once immediately on startup, before entering event loop
  render();
}

Renderer::~Renderer() {
  qDeleteAll(m_providers);
}

static QJsonArray renderFacts(QVector<Fact>& facts);

void Renderer::render() {
  QVector<Fact> facts;
  for (Provider* p: m_providers) {
    facts.append(p->render());
  }
  const QJsonDocument doc(renderFacts(facts));
  const QByteArray text = doc.toJson(QJsonDocument::Compact) + ",";
  puts(text.data());
  fflush(stdout);
}

static void addSeparatorToLastBlock(QJsonArray& blocks) {
  if (!blocks.isEmpty()) {
    const int idx = blocks.size() - 1;
    QJsonObject previousBlock = blocks[idx].toObject();
    previousBlock.insert(keySeparator, true);
    previousBlock.insert(keySeparatorBlockWidth, 15);
    blocks[idx] = previousBlock;
  }
}

static QJsonArray renderFacts(QVector<Fact>& facts) {
  std::stable_sort(facts.begin(), facts.end(),
    [](const Fact& x, const Fact& y) { return x.class_ < y.class_; });

  QJsonArray blocks;
  Fact::Class currentClass = (Fact::Class) -1;

  for (const Fact& fact: facts) {
    const QString& label = labelForFactClass(fact.class_);

    //when starting a new fact class...
    if (currentClass != fact.class_) {
      //...then...
      addSeparatorToLastBlock(blocks);

      //...and add a heading block
      QJsonObject block = {
        { keyName, label },
        { keyFullText, label },
        { keyColor, colorHeading },
        { keySeparator, false },
      };
      blocks.append(block);

      currentClass = fact.class_;
    }

    //add a block for this fact
    QJsonObject block = {
      { keyName, label },
      { keyColor, colorForFactPriority(fact.priority) },
      { keyFullText, fact.text },
      { keySeparator, false },
    };
    if (fact.priority == Fact::Danger) {
      block.insert(keyUrgent, true);
    }
    blocks.append(block);
  }

  //finally, add a clock
  addSeparatorToLastBlock(blocks);
  const QDateTime now = QDateTime::currentDateTime();
  blocks.append(QJsonObject({
    { keyName, nameClock },
    { keyInstance, instanceDate },
    { keyFullText, now.toString("yyyy-MM-dd") },
    { keyColor, colorDate },
    { keySeparatorBlockWidth, 6 },
    { keySeparator, false },
  }));
  blocks.append(QJsonObject({
    { keyName, nameClock },
    { keyInstance, instanceTime },
    { keyFullText, now.toString("hh:mm:ss") },
    { keyColor, colorTime },
    { keySeparator, false },
  }));

  return blocks;
}

#include "moc_renderer.cpp"
