#include "QSettingWnd.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QStackedLayout>
#include <QPushButton>
#include "QStyleSheetMgr.h"
#include <QMouseEvent>
#include "QSimpleSplit.h"
#include "QSettingAboutWnd.h"

QSettingWnd::QSettingWnd(QWidget* p /*= nullptr*/) : QWidget(p)
{
    m_centerWnd = new QWidget(this);
    m_centerWnd->setFixedSize(560, 480);
    m_centerWnd->setObjectName("QSettingWnd");
    QStyleSheetObject object;
    object.m_qssFileName = "./stylesheet/" + m_centerWnd->objectName() + ".qss";
    object.m_widget = m_centerWnd;
    QStyleSheetMgr::getMgr()->reg(object.m_qssFileName, object);

    setFixedSize(560, 480);
    // 设置为无边框及背景透明
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    m_vLayout = new QVBoxLayout(m_centerWnd);
    m_centerWnd->setLayout(m_vLayout);

    m_hTitleLayout = new QHBoxLayout(m_centerWnd);
    // 最小化按钮
    m_minBtn = new QPushButton();
    m_minBtn->setIcon(QPixmap("./img/minBtn_.png"));
    m_minBtn->setIconSize(QSize(20, 20));
    m_minBtn->setFixedSize(20, 20);

    // 关闭按钮
    m_closeBtn = new QPushButton();
    m_closeBtn->setIcon(QPixmap("./img/closeBtn_.png"));
    m_closeBtn->setIconSize(QSize(20, 20));
    m_closeBtn->setFixedSize(20, 20);

    // 添加弹簧
    m_hTitleLayout->addStretch();
    m_hTitleLayout->addWidget(m_minBtn);
    m_hTitleLayout->addWidget(m_closeBtn);

    m_vLayout->addLayout(m_hTitleLayout);

    m_hVerLayout = new QHBoxLayout(m_centerWnd);
    m_keyVerLabel = new QLabel("版本信息:");
    m_valueVerLabel = new QLabel();
    m_valueVerLabel->setText(WECHAT_VERSION);
    m_hVerLayout->addSpacing(25);
    m_hVerLayout->addWidget(m_keyVerLabel);
    m_hVerLayout->addSpacing(60);
    m_hVerLayout->addWidget(m_valueVerLabel);
    m_hVerLayout->addStretch();

    m_themeLayout = new QHBoxLayout(m_centerWnd);
    m_keyThemeLabel = new QLabel();
    m_keyThemeLabel->setText("主题设置:  ");
    m_valueThemeComboBox = new QComboBox(m_centerWnd);
    m_valueThemeComboBox->addItem("浅色主题");
    m_valueThemeComboBox->addItem("深色主题");

    m_themeLayout->addSpacing(25);
    m_themeLayout->addWidget(m_keyThemeLabel);
    m_themeLayout->addSpacing(50);
    m_themeLayout->addWidget(m_valueThemeComboBox);
    m_themeLayout->addStretch();


    m_vLayout->addLayout(m_themeLayout);
    m_vLayout->addLayout(m_hVerLayout);
    m_vLayout->addStretch();
}

void QSettingWnd::mouseMoveEvent(QMouseEvent* event)
{
    if (!m_leftBtnPressed)
    {
        return;
    }

    move(event->pos() - m_LeftBtnPressPoint + pos());
}

void QSettingWnd::mousePressEvent(QMouseEvent* event)
{
    m_leftBtnPressed = true;
    m_LeftBtnPressPoint = event->pos();
}

void QSettingWnd::mouseReleaseEvent(QMouseEvent* event)
{
    m_leftBtnPressed = false;
}
