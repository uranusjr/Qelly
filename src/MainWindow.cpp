#include "MainWindow.h"
#include <QApplication>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMenuBar>
#include <QTabBar>
#include <QToolBar>
#include <QStyle>
#include "SharedMenuBar.h"
#include "TabWidget.h"
#include <QDebug>

namespace UJ
{

namespace Qelly
{

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    buildToolBar();
    setMenuBar(SharedMenuBar::sharedInstance());
    _tabs = new TabWidget(this);
    _tabs->setTabPosition(QTabWidget::North);
    int cellWidth = 12;     // NOTE: Use global preferences
    int cellHeight = 25;    // NOTE: Use global preferences
    int row = 24;           // NOTE: Use global preferences
    int column = 80;        // NOTE: Use global preferences
    _tabs->setFixedSize(cellWidth * column,
                        cellHeight * (row + 1) + _tabs->tabBarHeight());
    setCentralWidget(_tabs);
    resize(_tabs->width(), _tabs->height() + _toolbar->height());
}

MainWindow::~MainWindow()
{
    delete _stretch;
    delete _inputFrame;
}

void MainWindow::buildToolBar()
{
    _stretch = new QWidget();
    _stretch->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    _address = new QLineEdit();
    _address->setPlaceholderText(tr("Input site address"));
    _address->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    _address->setFocusPolicy(Qt::ClickFocus);
    QLabel *inputLabel = new QLabel(QString("<small>") +
                                    tr("Address") +
                                    QString("</small>"));
    inputLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    inputLabel->setAlignment(Qt::AlignCenter);
    inputLabel->setFocusPolicy(Qt::NoFocus);
    _inputFrame = new QWidget();
    _inputFrame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    QVBoxLayout *inputLayout = new QVBoxLayout(_inputFrame);
    inputLayout->addWidget(_address);
    inputLayout->addWidget(inputLabel);
    _inputFrame->setLayout(inputLayout);

    QStyle *style = qApp->style();
    _toolbar = addToolBar(tr("General"));
    _toolbar->addAction(style->standardIcon(QStyle::SP_DriveNetIcon),
                        tr("Sites"));
    _toolbar->addAction(style->standardIcon(QStyle::SP_BrowserReload),
                        tr("Reconnect"));
    _toolbar->addAction(style->standardIcon(QStyle::SP_DialogSaveButton),
                        tr("Add"));
    _toolbar->addWidget(_inputFrame);
    _toolbar->addWidget(_stretch);
    _toolbar->addAction(style->standardIcon(QStyle::SP_DirIcon),
                        tr("Emicons"));
    _toolbar->addAction(style->standardIcon(QStyle::SP_DirIcon),
                        tr("Anti-Idle"));
    _toolbar->addAction(style->standardIcon(QStyle::SP_DirIcon),
                        tr("Peek"));
    _toolbar->addAction(style->standardIcon(QStyle::SP_DirIcon),
                        tr("Double Byte"));
    setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    setUnifiedTitleAndToolBarOnMac(true);
}

}   // namespace Qelly

}   // namespace UJ
