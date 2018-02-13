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

#include <time.h>
#include <unistd.h>
#include <QtCore/QCoreApplication>
#include <QtCore/QThread>

int main(int argc, char** argv) {
  QCoreApplication app(argc, argv);
  Renderer* r = new Renderer({
    buildNetworkProvider(),
    buildUpowerProvider(&app),
  }, &app);

  //every full second, wake up the renderer regardless of other events
  QThread::create([r] {
    for (;;) {
      //wait for next full second
      struct timespec ts;
      clock_gettime(CLOCK_REALTIME, &ts);
      usleep((1000000000 - ts.tv_nsec) / 1000);

      //call r->render() across thread boundaries
      QMetaObject::invokeMethod(r, &Renderer::render, Qt::QueuedConnection);
    }
  })->start();

  return app.exec();
}
