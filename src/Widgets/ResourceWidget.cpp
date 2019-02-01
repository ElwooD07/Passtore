#include "ResourceWidget.h"
#include <QtCore>
#include <QGraphicsBlurEffect>

ResourceWidget::ResourceWidget(QWidget* parent)
    : QWidget(parent)
{
    m_ui.setupUi(this);
    auto* effect = new QGraphicsBlurEffect(this);
    effect->setBlurHints(QGraphicsBlurEffect::QualityHint);
    effect->setBlurRadius(static_cast<qreal>(QFontInfo(m_ui.txtPassword->font()).pixelSize()) / 1.5);
    m_ui.txtPassword->setGraphicsEffect(effect);
}
