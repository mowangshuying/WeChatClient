#include "QEditLabel.h"
#include <QDebug>
#include <QApplication>
#include <QVector>
#include <vector>
#include <QMap>
#include <map>
#include <QKeyEvent>

QEditLabel::QEditLabel(QWidget* parent) : QWidget(parent)
{
    setObjectName("QEditLabel");

    m_StackLayout = new QStackedLayout(this);
    setLayout(m_StackLayout);

    m_label = new QLabel(this);
    m_label->setObjectName("TmpLabel");
    m_lineEdit = new QLineEdit(this);
    m_lineEdit->setObjectName("TmpLineEdit");
    m_label->setFocusPolicy(Qt::FocusPolicy::ClickFocus);

    m_label->installEventFilter(this);
    m_lineEdit->installEventFilter(this);

    m_StackLayout->addWidget(m_label);
    m_StackLayout->addWidget(m_lineEdit);
    m_StackLayout->setCurrentWidget(m_label);
}

void QEditLabel::setText(const char* text)
{
    if (m_label)
    {
        m_label->setText(text);
    }
}

QString QEditLabel::getText()
{
    if (m_label == nullptr)
        return "";

    return m_label->text();
}

bool QEditLabel::eventFilter(QObject* obj, QEvent* e)
{
    if (obj == m_label)
    {
        if (e->type() == QEvent::MouseButtonDblClick)
        {
            m_lineEdit->setText(m_label->text());
            m_StackLayout->setCurrentWidget(m_lineEdit);
        }
    }

    if (obj == m_lineEdit)
    {
        if (e->type() == QEvent::KeyPress)
        {
            QKeyEvent* keyEvent = static_cast<QKeyEvent*>(e);
            LogDebug << "key:" << keyEvent->key();
            if (keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return)
            {
                m_label->setText(m_lineEdit->text());
                m_StackLayout->setCurrentWidget(m_label);
                emit saveText();
            }
        }

        if (e->type() == QEvent::FocusOut)
        {
            m_label->setText(m_lineEdit->text());
            m_StackLayout->setCurrentWidget(m_label);
            emit saveText();
        }
    }

    return QWidget::eventFilter(obj, e);
}