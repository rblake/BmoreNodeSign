#!/usr/bin/env python

import Image
import serial
import struct
import numpy as np

WIDTH=64
HEIGHT=8

def find_arduino():
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
            #FIXME, opening this up is a kludge
            arduino = serial.Serial(device, 9600)
            del arduino
            return device
        except:    
            print "Failed to connect on",device     
    print "Could not find arduino!"
    return None
    

def open_arduino(baud=9600):
    return serial.Serial(find_arduino(), baud)

def convert_frame_for_arduino(frame):
    this_frame = np.reshape(
        np.fromstring(
            frame,
            dtype=np.uint8,
            count=WIDTH * HEIGHT * 3),
        (WIDTH, HEIGHT, 3)
        )
    s = "OK"
    pin_bulb_color = np.zeros((16,32,12),np.bool)
    for pin in xrange(0,16):
        for row in xrange(0,HEIGHT):
            for localcol in xrange(0,4):
                col = pin*4+localcol
                bulb = localcol*HEIGHT+row
                r0 = this_frame[col, row, 0]
                g0 = this_frame[col, row, 1]
                b0 = this_frame[col, row, 2]
                for bit in xrange(0,4):
                    pin_bulb_color[pin,bulb,  bit] = not (b0 & (0x01 << (4+bit)))
                    pin_bulb_color[pin,bulb,4+bit] = not (g0 & (0x01 << (4+bit)))
                    pin_bulb_color[pin,bulb,8+bit] = not (r0 & (0x01 << (4+bit)))
    bulb_color = np.zeros((32,12),np.uint16)
    for bulb in xrange(0,32):
        for color in xrange(0,12):
            word = np.uint16()
            for pin in xrange(0,16):
                if pin_bulb_color[pin,bulb,color]:
                    word |= (0x01 << pin)
            bulb_color[bulb,color] = word
            s += struct.pack('H', word)
    return s

def convert_image_to_frame(filename):
    im = Image.open(filename)
    im.thumbnail((WIDTH,HEIGHT),Image.ANTIALIAS)
    im = im.convert("RGB")
    (cols, rows) = im.size
    assert(rows == HEIGHT)
    assert(cols == WIDTH)
    assert(im.mode == "RGB")
    frame = ""
    for col in xrange(0,cols):
        for row in xrange(0,rows):
            (r0,g0,b0) = im.getpixel((col,row))
            frame += struct.pack('B', r0)
            frame += struct.pack('B', g0)
            frame += struct.pack('B', b0)
    return frame

def convert_image_for_arduino(filename):
    return convert_frame_for_arduino(convert_image_to_frame(filename))

if __name__=="__main__":
    import sys
    if sys.argv[1] == "--dump":
        image_bytes = convert_image_for_arduino(sys.argv[2])
        print "".join("\\x%x" % ord(c) for c in image_bytes[2:])
    elif sys.argv[1] == "--topipe":
        print convert_image_to_frame(sys.argv[2]),
    else:
        arduino = open_arduino()
        arduino.write(convert_image_for_arduino(sys.argv[1]))
