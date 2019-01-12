#pragma once
#include "ui_ResourceWidget.h"

class ResourceWidget: public QWidget
{
    Q_OBJECT
public:
    explicit ResourceWidget(QWidget* parent);

private:
    Ui::ResourceWidget m_ui;
};
