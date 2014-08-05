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
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Form
{
public:
    QGroupBox *groupBox;
    QLabel *label;
    QGroupBox *groupBox_2;
    QLabel *label_2;
    QGroupBox *groupBox_3;
    QLabel *label_3;
    QGroupBox *groupBox_4;
    QLabel *label_4;
    QGroupBox *groupBox_5;
    QGroupBox *groupBox_6;
    QLabel *label_5;
    QGroupBox *groupBox_7;
    QLabel *label_6;

    void setupUi(QWidget *Form)
    {
        if (Form->objectName().isEmpty())
            Form->setObjectName(QStringLiteral("Form"));
        Form->resize(632, 674);
        groupBox = new QGroupBox(Form);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        groupBox->setGeometry(QRect(20, 10, 591, 71));
        groupBox->setAlignment(Qt::AlignCenter);
        label = new QLabel(groupBox);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(10, 20, 591, 21));
        groupBox_2 = new QGroupBox(Form);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        groupBox_2->setGeometry(QRect(20, 90, 581, 81));
        groupBox_2->setAlignment(Qt::AlignCenter);
        label_2 = new QLabel(groupBox_2);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(40, 20, 201, 31));
        groupBox_3 = new QGroupBox(Form);
        groupBox_3->setObjectName(QStringLiteral("groupBox_3"));
        groupBox_3->setGeometry(QRect(20, 190, 581, 91));
        groupBox_3->setAlignment(Qt::AlignCenter);
        label_3 = new QLabel(groupBox_3);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(80, 50, 46, 13));
        groupBox_4 = new QGroupBox(Form);
        groupBox_4->setObjectName(QStringLiteral("groupBox_4"));
        groupBox_4->setGeometry(QRect(20, 300, 591, 91));
        groupBox_4->setAlignment(Qt::AlignCenter);
        label_4 = new QLabel(groupBox_4);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(70, 20, 46, 13));
        groupBox_5 = new QGroupBox(Form);
        groupBox_5->setObjectName(QStringLiteral("groupBox_5"));
        groupBox_5->setGeometry(QRect(20, 400, 591, 251));
        groupBox_5->setAlignment(Qt::AlignCenter);
        groupBox_6 = new QGroupBox(groupBox_5);
        groupBox_6->setObjectName(QStringLiteral("groupBox_6"));
        groupBox_6->setGeometry(QRect(20, 20, 291, 221));
        label_5 = new QLabel(groupBox_6);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setGeometry(QRect(100, 120, 46, 13));
        groupBox_7 = new QGroupBox(groupBox_5);
        groupBox_7->setObjectName(QStringLiteral("groupBox_7"));
        groupBox_7->setGeometry(QRect(330, 10, 241, 231));
        groupBox_7->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        label_6 = new QLabel(groupBox_7);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setGeometry(QRect(70, 80, 46, 13));

        retranslateUi(Form);

        QMetaObject::connectSlotsByName(Form);
    } // setupUi

    void retranslateUi(QWidget *Form)
    {
        Form->setWindowTitle(QApplication::translate("Form", "Form", 0));
        groupBox->setTitle(QApplication::translate("Form", "Introduction", 0));
        label->setText(QApplication::translate("Form", "TextLabel", 0));
        groupBox_2->setTitle(QApplication::translate("Form", "Handling Camera and Test Run File Settings", 0));
        label_2->setText(QApplication::translate("Form", "TextLabel", 0));
        groupBox_3->setTitle(QApplication::translate("Form", "Setting and retreiving Boundary Field", 0));
        label_3->setText(QApplication::translate("Form", "TextLabel", 0));
        groupBox_4->setTitle(QApplication::translate("Form", "Tweaking Track Settings", 0));
        label_4->setText(QApplication::translate("Form", "TextLabel", 0));
        groupBox_5->setTitle(QApplication::translate("Form", "Robotic Motion Control", 0));
        groupBox_6->setTitle(QApplication::translate("Form", "Manual Coil Control", 0));
        label_5->setText(QApplication::translate("Form", "TextLabel", 0));
        groupBox_7->setTitle(QApplication::translate("Form", "Automatic Waypoint Control", 0));
        label_6->setText(QApplication::translate("Form", "TextLabel", 0));
    } // retranslateUi

};

namespace Ui_help {
    class Help_window: public Ui_Form {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_HELP_WINDOW_H
