#include "Core/SimCore.h"
#include "../Engine/Engine.h"
#include <QDebug>
#include <cmath>

SimCore::SimCore(QObject* parent)
	:QObject(parent),
	pitch_deg(0.0),
	roll_deg(0.0),
	heading_deg(0.0),
	altitude_ft(1000.0),
	speed_kts(250)
{
	connect(&timer, &QTimer::timeout, this, &SimCore::update);
	timer.start(50);

	engine1.setThrottle(0.65);
	engine2.setThrottle(0.65);
}

void SimCore::update()
{
	static QElapsedTimer t;
	if (!t.isValid()) t.start();
	double dt = t.restart() / 1000.0;
	if (dt > 0.1) dt = 0.1;
	if (dt < 0.001) dt = 0.001;


	//FADEC
	//double throttle_cmd = 0.25;
	//fadec.control(engine1, throttle_cmd, altitude_ft);
	//fadec.control(engine2, throttle_cmd, altitude_ft);

	fadec.syncEngines(engine1, engine2);

	const double th = 0.25;
	engine1.update(dt, altitude_ft);
	engine2.update(dt, altitude_ft);

	double avgN1 = ((engine1.N1() + engine2.N1()) / 2.0);

	double thrust = (avgN1 - 30.0) * 0.3;
	if (thrust < 0.0) thrust = 0.0;

	double drag = 0.00005 * speed_kts * speed_kts;
	
	double accel = (thrust - drag) * 0.1;

	speed_kts += accel * dt;
	if (speed_kts < 0.0)
		speed_kts = 0.0;

	altitude_ft += std::sin(pitch_deg * M_PI / 180.0) * speed_kts*0.5 * dt;
	pitch_deg = 2.0 * sin(heading_deg * M_PI / 180.0);
	roll_deg = 5.0 * std::sin(heading_deg * M_PI / 90.0);

	heading_deg += 0.3;
	if (heading_deg > 360.0)
		heading_deg -= 360.0;

	emit updated();
}