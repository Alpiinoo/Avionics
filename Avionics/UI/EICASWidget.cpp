#include "EICASWidget.h"
#include <QPainter>
#include <QTimer>
#include <QFontDatabase>
#include <QKeyEvent>

EICASWidget::EICASWidget(Engine* eng1, Engine* eng2, QWidget* parent)
    : QWidget(parent), m_eng1(eng1), m_eng2(eng2)
{
    setWindowTitle("Avionics");
    resize(600, 400);
    setStyleSheet("background-color: black;");

    // Font
    QFont font("Lucida Console");
    if (!QFontDatabase::hasFamily("Lucida Console")) {
        font = QFont("Consolas");
    }
    font.setPointSize(14);
    font.setBold(false);
    setFont(font);

    // Timer
    QTimer* t = new QTimer(this);
    connect(t, &QTimer::timeout, this, QOverload<>::of(&EICASWidget::update));
    t->start(100);

    m_throttle = 0.5;
    setFocusPolicy(Qt::StrongFocus); 

}

void EICASWidget::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Plus || event->key() == Qt::Key_Equal) {
        m_throttle = qMin(1.0, m_throttle + 0.05); 
    }
    else if (event->key() == Qt::Key_Minus || event->key() == Qt::Key_Underscore) {
        m_throttle = qMax(0.0, m_throttle - 0.05);
    }

    m_eng1->setThrottle(m_throttle);
    m_eng2->setThrottle(m_throttle);

    QWidget::keyPressEvent(event);
}

void EICASWidget::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    auto drawEngine = [&](Engine* eng, int x, QString label)
        {
            p.setPen(QColor(0, 255, 0));  
            p.drawText(x, 40, label);

            auto colorForValue = [&](double value, double min, double max) {
                if (value > max) return QColor(255, 0, 0);   
                if (value < min) return QColor(255, 255, 0); 
                return QColor(0, 255, 0);                    
                };

            int y = 80;
            const int lineHeight = 40;

            // N1
            p.setPen(QColor(0, 255, 0));
            p.drawText(x, y, QString("N1: %1 %").arg(eng->N1(), 0, 'f', 1));
           // p.fillRect(x + 100, y - 15, eng->N1() * 2, 10, Qt::green);  // N1 bar

            // N2
            y += lineHeight;
            p.setPen(QColor(0, 255, 0));
            p.drawText(x, y, QString("N2: %1 %").arg(eng->N2(), 0, 'f', 1));
            //p.fillRect(x + 100, y - 15, eng->N2() * 2, 10, Qt::green);  // N2 bar

            // EGT
            y += lineHeight;
            double egt = eng->EGT();
            QColor egtColor = (egt > 750.0) ? QColor(255, 0, 0) : QColor(0, 255, 0);
            p.setPen(egtColor);
            p.drawText(x, y, QString("EGT: %1 \u00B0C").arg(egt, 0, 'f', 0));

            // Fuel Flow
            y += lineHeight;
            p.setPen(QColor(0, 255, 0));
            p.drawText(x, y, QString("FF: %1").arg(eng->FuelFlow(), 0, 'f', 1));

            // Oil Pressure
            y += lineHeight;
            QColor oilPColor = colorForValue(eng->OilPress(), 30, 250);
            p.setPen(oilPColor);
            p.drawText(x, y, QString("OIL P: %1").arg(eng->OilPress(), 0, 'f', 0));

            // Oil Temp
            y += lineHeight;
            QColor oilTColor = colorForValue(eng->OilTemp(), 40, 220);
            p.setPen(oilTColor);
            p.drawText(x, y, QString("OIL T: %1").arg(eng->OilTemp(), 0, 'f', 0));

            //Throttle Gecici
            p.setPen(Qt::cyan);
            p.setFont(QFont("Lucida Console", 14, QFont::Bold));
            p.drawText(width() / 2 - 80, height() - 20,
                QString("THR: %1%").arg(m_throttle * 100.0, 0, 'f', 1));
        };

    drawEngine(m_eng1, 80, "ENG1");
    drawEngine(m_eng2, 330, "ENG2");
}
