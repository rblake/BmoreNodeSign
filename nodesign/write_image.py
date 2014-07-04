#!/usr/bin/env python

import Image
import serial
import struct
import numpy as np

WIDTH=47
HEIGHT=24

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

    # Image like [AAAA BBB ]  needs to become
    # [ AAAA ]
    # [ BBB0 ]
    numStrings=48
    lightsPerString=24
    separated_frame = np.zeros((lightsPerString,numStrings,3),dtype=np.uint8)
    left_strand_count = 24 #change this to 23 depending on orientation.
    right_strand_count = (WIDTH-left_strand_count)
    separated_frame[0:left_strand_count,0:24,:] = this_frame[0:left_strand_count,:,:]
    separated_frame[0:right_strand_count,24:48,:] = this_frame[left_strand_count:WIDTH,:,:]

    s = "OK"
    pin_bulb_color = np.zeros((numStrings,lightsPerString,12),np.bool)
    for pin in xrange(0,numStrings):
        for bulb in xrange(0,lightsPerString):
            r0 = separated_frame[bulb, pin, 0]
            g0 = separated_frame[bulb, pin, 1]
            b0 = separated_frame[bulb, pin, 2]
            for bit in xrange(0,4):
                pin_bulb_color[pin,bulb,  bit] = not (b0 & (0x01 << (4+bit)))
                pin_bulb_color[pin,bulb,4+bit] = not (g0 & (0x01 << (4+bit)))
                pin_bulb_color[pin,bulb,8+bit] = not (r0 & (0x01 << (4+bit)))
    for bulb in xrange(0,lightsPerString):
        for color in xrange(0,12):
            for byte in xrange(0,numStrings,8):
                data = np.uint8()
                for pin in xrange(0, 8):
                    if pin_bulb_color[byte+pin,bulb,color]:
                        data |= (0x01 << pin)
                s += struct.pack('B', data)
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
        for start in xrange(2,len(image_bytes),24):
            print '"'+"".join("\\x%02x" % ord(c) for c in image_bytes[start:start+24])+'"'
    elif sys.argv[1] == "--topipe":
        print convert_image_to_frame(sys.argv[2]),
    else:
        arduino = open_arduino()
        arduino.write(convert_image_for_arduino(sys.argv[1]))
