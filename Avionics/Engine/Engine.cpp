#include "Engine/Engine.h"

Engine::Engine(QObject* parent)
    : QObject(parent),
    m_running(true),
    m_failCode(0),
    m_throttle(0.0),
    m_N1(20.0),
    m_N2(60.0),
    m_EGT(400.0),
    m_FuelFlow(300.0),
    m_OilPress(50.0),
    m_OilTemp(60.0)
{
}

void Engine::setThrottle(double t) {
    m_throttle = qBound(0.0, t, 1.0);
}

void Engine::shutdown() {
    m_running = false;
}

void Engine::restart() {
    m_running = true;
    if (m_N2 < 10.0) m_N2 = 10.0;
}

void Engine::update(double dt, double altitude_ft) {
    if (!m_running) {
        m_N1 = qMax(0.0, m_N1 - dt * 5.0);
        m_N2 = qMax(0.0, m_N2 - dt * 3.0);
        m_FuelFlow = 0;
        m_EGT = 200;
        m_OilPress = 0;
        emit changed();
        return;
    }

    double altFactor = 1.0 - (altitude_ft / 40000.0) * 0.3;
    if (altFactor < 0.5) altFactor = 0.5;

    double N1_target = m_throttle * N1_max * altFactor;
    double N2_target = (0.6 * m_throttle + 0.4) * N2_max;

    double alpha1 = dt / (tau_N1 + dt);
    double alpha2 = dt / (tau_N2 + dt);

    m_N1 += (N1_target - m_N1) * alpha1;
    m_N2 += (N2_target - m_N2) * alpha2;

    m_FuelFlow = 250 + 0.02 * m_N2 * m_throttle * 27000.0 / 1000.0;
    m_EGT = idleEGT + 250 * m_throttle + 0.1 * m_N2;
    m_OilPress = idleOil + (maxOil - idleOil) * (m_N2 / 100.0);
    m_OilTemp = 60 + 0.4 * (m_EGT - 400.0);

    if (m_EGT > 950.0) {
        m_running = false;
        m_failCode = 1;
        emit engineFailed(1);
    }

    if (m_OilPress < 20.0 && m_throttle > 0.4) {
        m_running = false;
        m_failCode = 2;
        emit engineFailed(2);
    }

    emit changed();
}