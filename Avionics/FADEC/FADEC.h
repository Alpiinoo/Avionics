#pragma once
#include <QObject>
#include "../Engine/Engine.h"

class FADEC :public QObject {
	Q_OBJECT
public:
	explicit FADEC(QObject* parent = nullptr);

	void control(Engine& eng, double throttle_cmd, double altitude_ft);
	void syncEngines(Engine& eng1, Engine& eng2);
	void setSyncEnabled(bool on) { m_syncEnabled = on; }

private:
	bool m_syncEnabled;
	double m_lastThrottle;
};