#pragma once
#include <QObject>
#include <cmath>
#include <QtGlobal>

class Engine : public QObject {
	Q_OBJECT
public:
	explicit Engine(QObject* parent = nullptr);

	void setThrottle(double t);
	void shutdown();
	void restart();
	void update(double dt, double altitude_ft);

	double N1() const { return m_N1; }
	double N2() const { return m_N2; }
	double EGT() const { return m_EGT; }
	double FuelFlow() const { return m_FuelFlow; }
	double OilPress() const { return m_OilPress; }
	double OilTemp() const { return m_OilTemp; }
	bool isRunning() const { return m_running; }
	int failureCode() const { return m_failCode; }

signals:
	void changed();
	void engineFailed(int code);

private:
	
	bool m_running;
	int m_failCode;
	double m_throttle;
	double m_N1, m_N2;
	double m_EGT, m_FuelFlow;
	double m_OilPress, m_OilTemp;

	static constexpr double N1_max = 100.0;
	static constexpr double N2_max = 100.0;
	static constexpr double tau_N1 = 5.0;
	static constexpr double tau_N2 = 7.5;
	static constexpr double idleEGT = 400.0;
	static constexpr double idleOil = 45.0;
	static constexpr double maxOil = 250.0;
};