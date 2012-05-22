#!/usr/bin/env python

import Image
import serial
import struct
import numpy as np

def open_arduino():
    locations=['/dev/ttyUSB0','/dev/ttyUSB1','/dev/ttyUSB2','/dev/ttyUSB3',  
               '/dev/ttyS0','/dev/ttyS1','/dev/ttyS2','/dev/ttyS3',
               'COM1', 'COM2', 'COM3', 'COM4', 'COM5', 'COM6', 'COM7', 'COM8']
    try:
        import serial.tools.list_ports as list_ports
        locations += [port[0] for port in list_ports.comports()]
    except:
        pass

    for device in locations:    
        try:    
            print "Trying...",device  
            arduino = serial.Serial(device, 9600)
            return arduino
        except:    
            print "Failed to connect on",device     
    print "Could not find arduino!"
    return None

def convert_image_for_arduino(filename):
    im = Image.open(filename)
    im.thumbnail((64,8),Image.ANTIALIAS)
    im = im.convert("RGB")
    (cols, rows) = im.size
    assert(rows == 8)
    assert(cols == 64)
    assert(im.mode == "RGB")
    s = "OK"
    bit_array = np.zeros((16,32,12),np.bool)
    for strand in xrange(0,16):
        for row in xrange(0,rows):
            for localcol in xrange(0,4):
                col = strand*4+localcol
                strandindex = localcol*8+row
                (r0,g0,b0) = im.getpixel((col,row))
                for bit in xrange(0,4):
                    bit_array[strand,strandindex,  bit] = not (b0 & (0x01 << (4+bit)))
                    bit_array[strand,strandindex,4+bit] = not (g0 & (0x01 << (4+bit)))
                    bit_array[strand,strandindex,8+bit] = not (r0 & (0x01 << (4+bit)))
    word_array = np.zeros((32,12),np.uint16)
    for ii in xrange(0,32):
        for jj in xrange(0,12):
            word = np.uint16()
            for kk in xrange(0,16):
                if bit_array[kk,ii,jj]:
                    word |= (0x01 << kk)
            s += struct.pack('H', word)
    return s

if __name__=="__main__":
    import sys
    print len(convert_image_for_arduino(sys.argv[1]))
    #arduino = open_arduino()
    #arduino.write(convert_image_for_arduino(sys.argv[1]))
