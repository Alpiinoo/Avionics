#pragma once
#include <QObject>
#include <QTimer>

class Simcore : public QObject
{
	Q_OBJECT

public:
	explicit SimCore(QObject* parent = nullptr);

	double pitch_deg;
	double roll_deg;
	double heading_deg;
	double altitude_ft;
	double speed_kts;

public slots:
	void update();

signals:
	void updated();

private:
	QTimer timer;
};