# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'color.ui'
#
# Created: Mon Nov 14 22:03:33 2011
#      by: PyQt4 UI code generator 4.7.2
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

class Ui_Color(object):
    def setupUi(self, Color):
        Color.setObjectName("Color")
        Color.resize(547, 505)
        self.centralwidget = QtGui.QWidget(Color)
        self.centralwidget.setObjectName("centralwidget")
        self.north = KColorCombo(self.centralwidget)
        self.north.setGeometry(QtCore.QRect(220, 60, 96, 31))
        self.north.setObjectName("north")
        self.west = KColorCombo(self.centralwidget)
        self.west.setGeometry(QtCore.QRect(30, 210, 96, 31))
        self.west.setObjectName("west")
        self.east = KColorCombo(self.centralwidget)
        self.east.setGeometry(QtCore.QRect(430, 210, 96, 31))
        self.east.setObjectName("east")
        self.south = KColorCombo(self.centralwidget)
        self.south.setGeometry(QtCore.QRect(240, 410, 96, 31))
        self.south.setObjectName("south")
        Color.setCentralWidget(self.centralwidget)
        self.menubar = QtGui.QMenuBar(Color)
        self.menubar.setGeometry(QtCore.QRect(0, 0, 547, 27))
        self.menubar.setObjectName("menubar")
        Color.setMenuBar(self.menubar)
        self.statusbar = QtGui.QStatusBar(Color)
        self.statusbar.setObjectName("statusbar")
        Color.setStatusBar(self.statusbar)

        self.retranslateUi(Color)
        QtCore.QMetaObject.connectSlotsByName(Color)

    def retranslateUi(self, Color):
        Color.setWindowTitle(QtGui.QApplication.translate("Color", "MainWindow", None, QtGui.QApplication.UnicodeUTF8))

from PyKDE4.kdeui import KColorCombo
