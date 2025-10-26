#include <QCoreApplication>
#include <QDebug>
#include "Core/SimCore.h"

int main(int argc, char* argv[])
{
	QCoreApplication app(argc, argv);

	SimCore sim;

	sim.engine1.setThrottle(1.0);
	sim.engine2.setThrottle(1.0);

	QObject::connect(&sim, &SimCore::updated, [&](){
		qDebug() << "ENG1 N1:" << sim.engine1.N1()
			<< "ENG2 N1:" << sim.engine2.N1()
			<< "SPD:" << sim.speed_kts
			<< "ALT:" << sim.altitude_ft;
	});

	return app.exec();
}