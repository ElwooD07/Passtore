#pragma once
#include "ui_ResourcesListWidget.h"

class ResourcesListWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ResourcesListWidget(QWidget* parent);

private:
    Ui::ResourceListWidget m_ui;
};

