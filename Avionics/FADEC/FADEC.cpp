#include "FADEC.h"
#include <cmath>
#include <QtGlobal>
#include <QRandomGenerator>

FADEC::FADEC(QObject* parent)
	:QObject(parent),
	m_syncEnabled(true),
	m_lastThrottle(0.0)
{
}

void FADEC::control(Engine& eng, double throttle_cmd, double altitude_ft)
{
	double filtered = 0.97 * m_lastThrottle + 0.03 * throttle_cmd;
	m_lastThrottle = filtered;

	if (eng.EGT() > 850.0)
		filtered -= 0.05;

	if (!eng.isRunning())
		filtered = 0.0;

	filtered = qBound(0.0, filtered, 1.0);

	eng.setThrottle(filtered);
}

void FADEC::syncEngines(Engine& e1, Engine& e2)
{
    if (!m_syncEnabled)
        return;

    double diff = e1.N1() - e2.N1();
    double avg = (e1.N1() + e2.N1()) * 0.5;

    if (qAbs(diff) > 0.3) {
        e1.forceSyncN1(avg);
        e2.forceSyncN1(avg);
    }

    else if (avg < 40.0) {
        double noise1 = (QRandomGenerator::global()->bounded(5) - 2) * 0.01; 
        double noise2 = (QRandomGenerator::global()->bounded(5) - 2) * 0.01;

        e1.forceSyncN1(e1.N1() + noise1);
        e2.forceSyncN1(e2.N1() + noise2);
    }

    else {
        e1.forceSyncN1(avg);
        e2.forceSyncN1(avg);
    }
}
