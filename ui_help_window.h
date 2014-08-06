/********************************************************************************
** Form generated from reading UI file 'help_window.ui'
**
** Created by: Qt User Interface Compiler version 5.3.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_HELP_WINDOW_H
#define UI_HELP_WINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Form
{
public:
    QGroupBox *groupBox;
    QLabel *label;
    QGroupBox *groupBox_2;
    QLabel *label_2;
    QLabel *label_7;
    QLabel *label_8;
    QLabel *label_9;
    QGroupBox *groupBox_3;
    QLabel *label_3;
    QLabel *label_10;
    QLabel *label_11;
    QLabel *label_12;
    QLabel *label_13;
    QGroupBox *groupBox_4;
    QLabel *label_4;
    QGroupBox *groupBox_5;
    QGroupBox *groupBox_6;
    QLabel *label_5;
    QGroupBox *groupBox_7;
    QLabel *label_6;
    QPushButton *helpwn_closepb;

    void setupUi(QWidget *Form)
    {
        if (Form->objectName().isEmpty())
            Form->setObjectName(QStringLiteral("Form"));
        Form->resize(991, 877);
        QPalette palette;
        QBrush brush(QColor(85, 0, 127, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::WindowText, brush);
        palette.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        QBrush brush1(QColor(106, 104, 100, 255));
        brush1.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Disabled, QPalette::WindowText, brush1);
        Form->setPalette(palette);
        groupBox = new QGroupBox(Form);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        groupBox->setGeometry(QRect(50, 30, 891, 101));
        QPalette palette1;
        QBrush brush2(QColor(235, 0, 0, 255));
        brush2.setStyle(Qt::SolidPattern);
        palette1.setBrush(QPalette::Active, QPalette::WindowText, brush2);
        QBrush brush3(QColor(0, 0, 255, 255));
        brush3.setStyle(Qt::SolidPattern);
        palette1.setBrush(QPalette::Active, QPalette::Text, brush3);
        palette1.setBrush(QPalette::Inactive, QPalette::WindowText, brush2);
        palette1.setBrush(QPalette::Inactive, QPalette::Text, brush3);
        palette1.setBrush(QPalette::Disabled, QPalette::WindowText, brush1);
        palette1.setBrush(QPalette::Disabled, QPalette::Text, brush1);
        groupBox->setPalette(palette1);
        QFont font;
        font.setBold(true);
        font.setWeight(75);
        groupBox->setFont(font);
        groupBox->setAlignment(Qt::AlignCenter);
        label = new QLabel(groupBox);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(10, 20, 861, 61));
        QPalette palette2;
        QBrush brush4(QColor(85, 0, 255, 255));
        brush4.setStyle(Qt::SolidPattern);
        palette2.setBrush(QPalette::Active, QPalette::WindowText, brush4);
        palette2.setBrush(QPalette::Active, QPalette::Text, brush4);
        palette2.setBrush(QPalette::Inactive, QPalette::WindowText, brush4);
        palette2.setBrush(QPalette::Inactive, QPalette::Text, brush4);
        palette2.setBrush(QPalette::Disabled, QPalette::WindowText, brush1);
        palette2.setBrush(QPalette::Disabled, QPalette::Text, brush1);
        label->setPalette(palette2);
        QFont font1;
        font1.setBold(false);
        font1.setWeight(50);
        label->setFont(font1);
        label->setWordWrap(true);
        groupBox_2 = new QGroupBox(Form);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        groupBox_2->setGeometry(QRect(40, 130, 921, 121));
        QPalette palette3;
        QBrush brush5(QColor(170, 0, 0, 255));
        brush5.setStyle(Qt::SolidPattern);
        palette3.setBrush(QPalette::Active, QPalette::WindowText, brush5);
        palette3.setBrush(QPalette::Inactive, QPalette::WindowText, brush5);
        palette3.setBrush(QPalette::Disabled, QPalette::WindowText, brush1);
        groupBox_2->setPalette(palette3);
        groupBox_2->setFont(font);
        groupBox_2->setAlignment(Qt::AlignCenter);
        label_2 = new QLabel(groupBox_2);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(20, 10, 821, 31));
        QPalette palette4;
        QBrush brush6(QColor(0, 0, 127, 255));
        brush6.setStyle(Qt::SolidPattern);
        palette4.setBrush(QPalette::Active, QPalette::WindowText, brush6);
        palette4.setBrush(QPalette::Inactive, QPalette::WindowText, brush6);
        palette4.setBrush(QPalette::Disabled, QPalette::WindowText, brush1);
        label_2->setPalette(palette4);
        label_2->setFont(font1);
        label_2->setWordWrap(true);
        label_7 = new QLabel(groupBox_2);
        label_7->setObjectName(QStringLiteral("label_7"));
        label_7->setGeometry(QRect(20, 30, 931, 16));
        QPalette palette5;
        palette5.setBrush(QPalette::Active, QPalette::WindowText, brush6);
        palette5.setBrush(QPalette::Inactive, QPalette::WindowText, brush6);
        palette5.setBrush(QPalette::Disabled, QPalette::WindowText, brush1);
        label_7->setPalette(palette5);
        label_7->setFont(font1);
        label_8 = new QLabel(groupBox_2);
        label_8->setObjectName(QStringLiteral("label_8"));
        label_8->setGeometry(QRect(20, 46, 841, 20));
        QPalette palette6;
        palette6.setBrush(QPalette::Active, QPalette::WindowText, brush6);
        palette6.setBrush(QPalette::Inactive, QPalette::WindowText, brush6);
        palette6.setBrush(QPalette::Disabled, QPalette::WindowText, brush1);
        label_8->setPalette(palette6);
        label_8->setFont(font1);
        label_9 = new QLabel(groupBox_2);
        label_9->setObjectName(QStringLiteral("label_9"));
        label_9->setGeometry(QRect(20, 70, 901, 41));
        QPalette palette7;
        palette7.setBrush(QPalette::Active, QPalette::WindowText, brush6);
        palette7.setBrush(QPalette::Inactive, QPalette::WindowText, brush6);
        palette7.setBrush(QPalette::Disabled, QPalette::WindowText, brush1);
        label_9->setPalette(palette7);
        label_9->setFont(font1);
        label_9->setWordWrap(true);
        groupBox_3 = new QGroupBox(Form);
        groupBox_3->setObjectName(QStringLiteral("groupBox_3"));
        groupBox_3->setGeometry(QRect(20, 260, 961, 191));
        QPalette palette8;
        QBrush brush7(QColor(120, 0, 180, 255));
        brush7.setStyle(Qt::SolidPattern);
        palette8.setBrush(QPalette::Active, QPalette::WindowText, brush7);
        palette8.setBrush(QPalette::Inactive, QPalette::WindowText, brush7);
        palette8.setBrush(QPalette::Disabled, QPalette::WindowText, brush1);
        groupBox_3->setPalette(palette8);
        groupBox_3->setFont(font);
        groupBox_3->setAlignment(Qt::AlignCenter);
        label_3 = new QLabel(groupBox_3);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(20, 20, 851, 16));
        QPalette palette9;
        QBrush brush8(QColor(150, 100, 0, 255));
        brush8.setStyle(Qt::SolidPattern);
        palette9.setBrush(QPalette::Active, QPalette::WindowText, brush8);
        palette9.setBrush(QPalette::Inactive, QPalette::WindowText, brush8);
        palette9.setBrush(QPalette::Disabled, QPalette::WindowText, brush1);
        label_3->setPalette(palette9);
        label_3->setFont(font1);
        label_3->setWordWrap(true);
        label_10 = new QLabel(groupBox_3);
        label_10->setObjectName(QStringLiteral("label_10"));
        label_10->setGeometry(QRect(20, 40, 891, 31));
        QPalette palette10;
        palette10.setBrush(QPalette::Active, QPalette::WindowText, brush8);
        palette10.setBrush(QPalette::Inactive, QPalette::WindowText, brush8);
        palette10.setBrush(QPalette::Disabled, QPalette::WindowText, brush1);
        label_10->setPalette(palette10);
        label_10->setFont(font1);
        label_10->setWordWrap(true);
        label_11 = new QLabel(groupBox_3);
        label_11->setObjectName(QStringLiteral("label_11"));
        label_11->setGeometry(QRect(20, 70, 891, 31));
        QPalette palette11;
        palette11.setBrush(QPalette::Active, QPalette::WindowText, brush8);
        palette11.setBrush(QPalette::Inactive, QPalette::WindowText, brush8);
        palette11.setBrush(QPalette::Disabled, QPalette::WindowText, brush1);
        label_11->setPalette(palette11);
        label_11->setFont(font1);
        label_11->setWordWrap(true);
        label_12 = new QLabel(groupBox_3);
        label_12->setObjectName(QStringLiteral("label_12"));
        label_12->setGeometry(QRect(20, 100, 881, 41));
        QPalette palette12;
        palette12.setBrush(QPalette::Active, QPalette::WindowText, brush8);
        palette12.setBrush(QPalette::Inactive, QPalette::WindowText, brush8);
        palette12.setBrush(QPalette::Disabled, QPalette::WindowText, brush1);
        label_12->setPalette(palette12);
        label_12->setFont(font1);
        label_12->setWordWrap(true);
        label_13 = new QLabel(groupBox_3);
        label_13->setObjectName(QStringLiteral("label_13"));
        label_13->setGeometry(QRect(20, 140, 901, 31));
        QPalette palette13;
        palette13.setBrush(QPalette::Active, QPalette::WindowText, brush8);
        palette13.setBrush(QPalette::Inactive, QPalette::WindowText, brush8);
        palette13.setBrush(QPalette::Disabled, QPalette::WindowText, brush1);
        label_13->setPalette(palette13);
        label_13->setFont(font1);
        label_13->setWordWrap(true);
        groupBox_4 = new QGroupBox(Form);
        groupBox_4->setObjectName(QStringLiteral("groupBox_4"));
        groupBox_4->setGeometry(QRect(80, 480, 651, 101));
        groupBox_4->setFont(font);
        groupBox_4->setAlignment(Qt::AlignCenter);
        label_4 = new QLabel(groupBox_4);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(70, 20, 46, 13));
        label_4->setFont(font1);
        label_4->setWordWrap(true);
        groupBox_5 = new QGroupBox(Form);
        groupBox_5->setObjectName(QStringLiteral("groupBox_5"));
        groupBox_5->setGeometry(QRect(50, 610, 651, 251));
        groupBox_5->setFont(font);
        groupBox_5->setAlignment(Qt::AlignCenter);
        groupBox_6 = new QGroupBox(groupBox_5);
        groupBox_6->setObjectName(QStringLiteral("groupBox_6"));
        groupBox_6->setGeometry(QRect(20, 20, 291, 221));
        label_5 = new QLabel(groupBox_6);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setGeometry(QRect(100, 120, 46, 13));
        label_5->setFont(font1);
        label_5->setWordWrap(true);
        groupBox_7 = new QGroupBox(groupBox_5);
        groupBox_7->setObjectName(QStringLiteral("groupBox_7"));
        groupBox_7->setGeometry(QRect(400, 10, 241, 231));
        groupBox_7->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        label_6 = new QLabel(groupBox_7);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setGeometry(QRect(70, 80, 46, 13));
        label_6->setFont(font1);
        label_6->setWordWrap(true);
        helpwn_closepb = new QPushButton(Form);
        helpwn_closepb->setObjectName(QStringLiteral("helpwn_closepb"));
        helpwn_closepb->setGeometry(QRect(670, 10, 75, 23));

        retranslateUi(Form);

        QMetaObject::connectSlotsByName(Form);
    } // setupUi

    void retranslateUi(QWidget *Form)
    {
        Form->setWindowTitle(QApplication::translate("Form", "Form", 0));
        groupBox->setTitle(QApplication::translate("Form", "Introduction", 0));
        label->setText(QApplication::translate("Form", "This is a software interface to track and control magentic microrobots, maunally and automatically by setting waypoints. The tracking proces entails the user to separate the boundary contour from the precessing image and enter tracking details for tracking the microrobot in motion. The robotic motion image frames and data obtained can be stored in new or existing video and text files respectively. This GUI prides manual control of robotic motion directly through commands while automatic control through the settings of specific path waypoints. This manual page explains eachsection of the GUI for the implemetation process.", 0));
        groupBox_2->setTitle(QApplication::translate("Form", "Handling Camera and Test Run File Settings", 0));
        label_2->setText(QApplication::translate("Form", "-  The camera settings in the top above the camera streamed images windowconfigures the camera video format, in pixel resolution and type, and frame rate per second", 0));
        label_7->setText(QApplication::translate("Form", "- The camera settings can be chosen from the drop down menus for the corresponding sub sections box", 0));
        label_8->setText(QApplication::translate("Form", "- The checkboxes include options for tracking the microrobot (based on settings entered in Robot Track Settings box) and file settings for saving the real-time calculated data along with video frames. ", 0));
        label_9->setText(QApplication::translate("Form", "- A text file can be created, overwritten, or appended to at the end to store the position co-ordinates and orientation data of the robotic motion tracked. The interface provides features of 'auto-incrementing' the preivously entered text filename for storing data. This cannot be checked along with 'append to file' checkbox as only one feature can be implemented for a particulr feature implementation", 0));
        groupBox_3->setTitle(QApplication::translate("Form", "Setting and Retreiving Boundary Field", 0));
        label_3->setText(QApplication::translate("Form", "- Boundary Field Detection and Removal Section implements the contour detection of boundary wall in the test bed after user input image clicked boundary points", 0));
        label_10->setText(QApplication::translate("Form", "- The user is required to check the checkbox to enter the number of boundary points to provide the interface. This number can be dynamically changed to suit the necessity of boundary points for the most accurate and optimum detection of boundary contours", 0));
        label_11->setText(QApplication::translate("Form", "-The number of points lciked on the imsge apper a a number in the  label box, adjacent to the the checkbox. This number returns to 0 after the total number of clicks, mentioned in the user input text box, has been achieved. ", 0));
        label_12->setText(QApplication::translate("Form", "- The user should enter the threshold values for thresholding the video frames for boundary contours detection. The type of masking (or boundary walls removal) can be filled (i.e. region,between boundary walls, inclusive) or non-filled (ie. containing only discrete boundary contours ). ", 0));
        label_13->setText(QApplication::translate("Form", "- Once the threshold limits are applied, the boundary contours and binary threshold masked images appear on the lower panel of frame processing images to enable user to evaluate the limit values used. The process should be repeated till the boundary contours are  not disctinly  detected for effective tracking of microrobot   ", 0));
        groupBox_4->setTitle(QApplication::translate("Form", "Tweaking Track Settings", 0));
        label_4->setText(QApplication::translate("Form", "TextLabel", 0));
        groupBox_5->setTitle(QApplication::translate("Form", "Robotic Motion Control", 0));
        groupBox_6->setTitle(QApplication::translate("Form", "Manual Coil Control", 0));
        label_5->setText(QApplication::translate("Form", "TextLabel", 0));
        groupBox_7->setTitle(QApplication::translate("Form", "Automatic Waypoint Control", 0));
        label_6->setText(QApplication::translate("Form", "TextLabel", 0));
        helpwn_closepb->setText(QApplication::translate("Form", "Close Window", 0));
    } // retranslateUi

};

namespace Ui {
    class Form: public Ui_Form {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_HELP_WINDOW_H
