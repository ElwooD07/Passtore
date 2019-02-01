#pragma once
#include "ui_ResourcesListWidget.h"
#include "ResourcesModel.h"

class ResourcesListWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ResourcesListWidget(QWidget* parent, ResourcesModel* model);

private:
    Ui::ResourceListWidget m_ui;
};

