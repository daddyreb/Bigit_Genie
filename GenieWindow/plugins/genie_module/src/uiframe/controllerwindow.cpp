/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "stable.h"



//! [0]
ControllerWindow::ControllerWindow(QWidget*parent)
    :QWidget(parent)
{
  //  previewWindow = new PreviewWindow(this);

    createTypeGroupBox();
    createHintsGroupBox();

    quitButton = new QPushButton(tr("&Quit"));
    connect(quitButton, SIGNAL(clicked()), qApp, SLOT(quit()));

    QHBoxLayout *bottomLayout = new QHBoxLayout;
    bottomLayout->addStretch();
    bottomLayout->addWidget(quitButton);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(typeGroupBox);
    mainLayout->addWidget(hintsGroupBox);
    mainLayout->addLayout(bottomLayout);
    setLayout(mainLayout);

    setWindowTitle(tr("Window Flags"));
  //  updatePreview();
}
//! [0]

//! [1]
void ControllerWindow::updatePreview()
{
    Qt::WindowFlags flags = 0;

    if (windowRadioButton->isChecked()) {
        flags = Qt::Window;
    } else if (dialogRadioButton->isChecked()) {
        flags = Qt::Dialog;
    } else if (sheetRadioButton->isChecked()) {
        flags = Qt::Sheet;
    } else if (drawerRadioButton->isChecked()) {
        flags = Qt::Drawer;
    } else if (popupRadioButton->isChecked()) {
        flags = Qt::Popup;
    } else if (toolRadioButton->isChecked()) {
        flags = Qt::Tool;
    } else if (toolTipRadioButton->isChecked()) {
        flags = Qt::ToolTip;
    } else if (splashScreenRadioButton->isChecked()) {
        flags = Qt::SplashScreen;
//! [1] //! [2]
    }
//! [2] //! [3]

    if (msWindowsFixedSizeDialogCheckBox->isChecked())
        flags |= Qt::MSWindowsFixedSizeDialogHint;
    if (x11BypassWindowManagerCheckBox->isChecked())
        flags |= Qt::X11BypassWindowManagerHint;
    if (framelessWindowCheckBox->isChecked())
        flags |= Qt::FramelessWindowHint;
    if (windowTitleCheckBox->isChecked())
        flags |= Qt::WindowTitleHint;
    if (windowSystemMenuCheckBox->isChecked())
        flags |= Qt::WindowSystemMenuHint;
    if (windowMinimizeButtonCheckBox->isChecked())
        flags |= Qt::WindowMinimizeButtonHint;
    if (windowMaximizeButtonCheckBox->isChecked())
        flags |= Qt::WindowMaximizeButtonHint;
    if (windowCloseButtonCheckBox->isChecked())
        flags |= Qt::WindowCloseButtonHint;
    if (windowContextHelpButtonCheckBox->isChecked())
        flags |= Qt::WindowContextHelpButtonHint;
    if (windowShadeButtonCheckBox->isChecked())
        flags |= Qt::WindowShadeButtonHint;
    if (windowStaysOnTopCheckBox->isChecked())
        flags |= Qt::WindowStaysOnTopHint;
    if (windowStaysOnBottomCheckBox->isChecked())
        flags |= Qt::WindowStaysOnBottomHint;
    if (customizeWindowHintCheckBox->isChecked())
        flags |= Qt::CustomizeWindowHint;

    QGenieFrameMain *main=QGenieFrameMain::GetSingletonPtr();
    ASSERT(main);
    main->setWindowFlags(flags);

    QGenieFrameDebug *dbgpan=QGenieFrameDebug::GetSingletonPtr();
    ASSERT(dbgpan);
    dbgpan->mUi.textEdit->setText(getText(flags));
/*
    previewWindow->setWindowFlags(flags);
//! [3] //! [4]
 */
    QPoint pos = main->pos();
    if (pos.x() < 0)
        pos.setX(0);
    if (pos.y() < 0)
        pos.setY(0);
    main->move(pos);
    main->show();

}

QString ControllerWindow::getText(Qt::WindowFlags flags)
{
    QString text;

    Qt::WindowFlags type = (flags & Qt::WindowType_Mask);
    if (type == Qt::Window) {
        text = "Qt::Window";
    } else if (type == Qt::Dialog) {
        text = "Qt::Dialog";
    } else if (type == Qt::Sheet) {
        text = "Qt::Sheet";
    } else if (type == Qt::Drawer) {
        text = "Qt::Drawer";
    } else if (type == Qt::Popup) {
        text = "Qt::Popup";
    } else if (type == Qt::Tool) {
        text = "Qt::Tool";
    } else if (type == Qt::ToolTip) {
        text = "Qt::ToolTip";
    } else if (type == Qt::SplashScreen) {
        text = "Qt::SplashScreen";
    }

    if (flags & Qt::MSWindowsFixedSizeDialogHint)
        text += "\n| Qt::MSWindowsFixedSizeDialogHint";
    if (flags & Qt::X11BypassWindowManagerHint)
        text += "\n| Qt::X11BypassWindowManagerHint";
    if (flags & Qt::FramelessWindowHint)
        text += "\n| Qt::FramelessWindowHint";
    if (flags & Qt::WindowTitleHint)
        text += "\n| Qt::WindowTitleHint";
    if (flags & Qt::WindowSystemMenuHint)
        text += "\n| Qt::WindowSystemMenuHint";
    if (flags & Qt::WindowMinimizeButtonHint)
        text += "\n| Qt::WindowMinimizeButtonHint";
    if (flags & Qt::WindowMaximizeButtonHint)
        text += "\n| Qt::WindowMaximizeButtonHint";
    if (flags & Qt::WindowCloseButtonHint)
        text += "\n| Qt::WindowCloseButtonHint";
    if (flags & Qt::WindowContextHelpButtonHint)
        text += "\n| Qt::WindowContextHelpButtonHint";
    if (flags & Qt::WindowShadeButtonHint)
        text += "\n| Qt::WindowShadeButtonHint";
    if (flags & Qt::WindowStaysOnTopHint)
        text += "\n| Qt::WindowStaysOnTopHint";
    if (flags & Qt::CustomizeWindowHint)
        text += "\n| Qt::CustomizeWindowHint";

    return text;
}

//! [4]

//! [5]
void ControllerWindow::createTypeGroupBox()
{
    typeGroupBox = new QGroupBox(tr("Type"));

    windowRadioButton = createRadioButton(tr("Window"));
    dialogRadioButton = createRadioButton(tr("Dialog"));
    sheetRadioButton = createRadioButton(tr("Sheet"));
    drawerRadioButton = createRadioButton(tr("Drawer"));
    popupRadioButton = createRadioButton(tr("Popup"));
    toolRadioButton = createRadioButton(tr("Tool"));
    toolTipRadioButton = createRadioButton(tr("Tooltip"));
    splashScreenRadioButton = createRadioButton(tr("Splash screen"));
    windowRadioButton->setChecked(true);

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(windowRadioButton, 0, 0);
    layout->addWidget(dialogRadioButton, 1, 0);
    layout->addWidget(sheetRadioButton, 2, 0);
    layout->addWidget(drawerRadioButton, 3, 0);
    layout->addWidget(popupRadioButton, 0, 1);
    layout->addWidget(toolRadioButton, 1, 1);
    layout->addWidget(toolTipRadioButton, 2, 1);
    layout->addWidget(splashScreenRadioButton, 3, 1);
    typeGroupBox->setLayout(layout);
}
//! [5]

//! [6]
void ControllerWindow::createHintsGroupBox()
{
    hintsGroupBox = new QGroupBox(tr("Hints"));

    msWindowsFixedSizeDialogCheckBox =
            createCheckBox(tr("MS Windows fixed size dialog"));
    x11BypassWindowManagerCheckBox =
            createCheckBox(tr("X11 bypass window manager"));
    framelessWindowCheckBox = createCheckBox(tr("Frameless window"));
    windowTitleCheckBox = createCheckBox(tr("Window title"));
    windowSystemMenuCheckBox = createCheckBox(tr("Window system menu"));
    windowMinimizeButtonCheckBox = createCheckBox(tr("Window minimize button"));
    windowMaximizeButtonCheckBox = createCheckBox(tr("Window maximize button"));
    windowCloseButtonCheckBox = createCheckBox(tr("Window close button"));
    windowContextHelpButtonCheckBox =
            createCheckBox(tr("Window context help button"));
    windowShadeButtonCheckBox = createCheckBox(tr("Window shade button"));
    windowStaysOnTopCheckBox = createCheckBox(tr("Window stays on top"));
    windowStaysOnBottomCheckBox = createCheckBox(tr("Window stays on bottom"));
    customizeWindowHintCheckBox= createCheckBox(tr("Customize window"));

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(msWindowsFixedSizeDialogCheckBox, 0, 0);
    layout->addWidget(x11BypassWindowManagerCheckBox, 1, 0);
    layout->addWidget(framelessWindowCheckBox, 2, 0);
    layout->addWidget(windowTitleCheckBox, 3, 0);
    layout->addWidget(windowSystemMenuCheckBox, 4, 0);
    layout->addWidget(windowMinimizeButtonCheckBox, 0, 1);
    layout->addWidget(windowMaximizeButtonCheckBox, 1, 1);
    layout->addWidget(windowCloseButtonCheckBox, 2, 1);
    layout->addWidget(windowContextHelpButtonCheckBox, 3, 1);
    layout->addWidget(windowShadeButtonCheckBox, 4, 1);
    layout->addWidget(windowStaysOnTopCheckBox, 5, 1);
    layout->addWidget(windowStaysOnBottomCheckBox, 6, 1);
    layout->addWidget(customizeWindowHintCheckBox, 5, 0);
    hintsGroupBox->setLayout(layout);
}
//! [6]

//! [7]
QCheckBox *ControllerWindow::createCheckBox(const QString &text)
{
    QCheckBox *checkBox = new QCheckBox(text);
    connect(checkBox, SIGNAL(clicked()), this, SLOT(updatePreview()));
    return checkBox;
}
//! [7]

//! [8]
QRadioButton *ControllerWindow::createRadioButton(const QString &text)
{
    QRadioButton *button = new QRadioButton(text);
    connect(button, SIGNAL(clicked()), this, SLOT(updatePreview()));
    return button;
}
//! [8]
