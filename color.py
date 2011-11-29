

from ui_color import Ui_Color
from write_bitmap import open_arduino

import serial
import struct

# QT
from PyQt4.QtCore import *
from PyQt4.QtGui import *

class Color(QMainWindow,Ui_Color):
    def __init__(self):
        QMainWindow.__init__(self);
        Ui_Color.__init__(self);
        self.setupUi(self)

        self.arduino = open_arduino()

        white = QColor(255,255,255)
        self.colors = {}
        self.colors["north"] = white
        self.colors["south"] = white
        self.colors["east"] = white
        self.colors["west"] = white



        self.north.activated.connect(lambda c,self=self: self.changeColor("north",c))
        self.south.activated.connect(lambda c,self=self: self.changeColor("south",c))
        self.east.activated.connect(lambda c,self=self: self.changeColor("east",c))
        self.west.activated.connect(lambda c,self=self: self.changeColor("west",c))


    def changeColor(self, direction, color):
        self.colors[direction] = color
        self.sendData()

    def sendData(self):
        for i in "north","south","east","west":
            self.arduino.write(struct.pack("B",self.colors[i].red()))
            self.arduino.write(struct.pack("B",self.colors[i].green()))
            self.arduino.write(struct.pack("B",self.colors[i].blue()))
            print i,self.colors[i].red(),self.colors[i].green(),self.colors[i].blue()

if __name__=="__main__":
    import sys
    a = QApplication(sys.argv)
    w = Color()
    #w.showMaximized()
    w.show()
    a.exec_()
