#pragma once
#include <QWidget>
#include <QPainter>
#include <QKeyEvent>
#include "../Engine/Engine.h"

class EICASWidget : public QWidget
{
    Q_OBJECT

public:
    explicit EICASWidget(Engine* left, Engine* right, QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private:
    Engine* m_eng1;
    Engine* m_eng2;

    double m_throttle;
};
