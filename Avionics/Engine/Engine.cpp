#include "Engine/Engine.h"

Engine::Engine(QObject* parent)
    : QObject(parent),
    m_running(true),
    m_failCode(0),
    m_throttle(0.0),
    m_N1(30.0),
    m_N2(60.0),
    m_EGT(400.0),
    m_FuelFlow(300.0),
    m_OilPress(50.0),
    m_OilTemp(60.0)
{
}

void Engine::setThrottle(double t) {
    const double idle = 0.25;
    m_throttle = qBound(0.0, t, 1.0);
    m_throttle = idle + m_throttle * (1.0 - idle);
}

void Engine::shutdown() {
    m_running = false;
}

void Engine::restart() {
    m_running = true;
    if (m_N2 < 10.0) m_N2 = 10.0;
}

//FADEC
void Engine::forceSyncN1(double n1)
{
    m_N1 = n1;
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

    // --- FADEC BOOST LOGIC ---
    double N1_target = m_throttle * N1_max * altFactor;

    //%105 spool overshoot
    if (m_throttle > 0.98)
        N1_target *= 1.05;

    double N2_target = (0.6 * m_throttle + 0.4) * N2_max;

    // inertia filter
    double alpha1 = dt / (tau_N1 + dt);
    double alpha2 = dt / (tau_N2 + dt);

    m_N1 += (N1_target - m_N1) * alpha1;
    m_N2 += (N2_target - m_N2) * alpha2;

    m_FuelFlow = 250 + 0.02 * m_N2 * m_throttle * 27000.0 / 1000.0;
    double targetEGT = idleEGT + 600.0 * m_throttle + 0.3 * (m_N2 - 60.0);
    double heatingRate = (targetEGT > m_EGT) ? 0.15 : 0.25; //inertia
    m_EGT += (targetEGT - m_EGT) * heatingRate * dt;
    if (m_throttle > 0.98)
        m_EGT += 0.5; 
    m_OilPress = idleOil + (maxOil - idleOil) * (m_N2 / 100.0);
    m_OilTemp = 60 + 0.4 * (m_EGT - 400.0);

    /*if (m_EGT > 1550.0) {
        m_running = false;
        m_failCode = 1;
        emit engineFailed(1);
    }*/

    if (m_OilPress < 20.0 && m_throttle > 0.4) {
        m_running = false;
        m_failCode = 2;
        emit engineFailed(2);
    }

    emit changed();
}