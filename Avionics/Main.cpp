#include <QCoreApplication>
#include <qapplication>
#include <QDebug>
#include "Core/SimCore.h"
#include "UI/EICASWidget.h"

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);

	SimCore sim;

	sim.engine1.setThrottle(0.5);
	sim.engine2.setThrottle(0.5);

	EICASWidget eicas(&sim.engine1, &sim.engine2);
	eicas.show();

	/*QObject::connect(&sim, &SimCore::updated, [&]() {
		qDebug() << "ENG1 N1:" << sim.engine1.N1()
			<< "ENG2 N1:" << sim.engine2.N1()
			<< "SPD:" << sim.speed_kts
			<< "ALT:" << sim.altitude_ft;
	});*/

	return app.exec();
}