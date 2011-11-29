import Image
import serial
import struct

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
    im.thumbnail((8,8),Image.ANTIALIAS)
    im.convert("RGB")
    (rows, cols) = im.size
    assert(rows == 8)
    assert(cols == 8)
    assert(im.mode == "RGB")
    s = ""
    for row in xrange(0,8):
        for col in [0,2,4,6]:
            (r0,g0,b0) = im.getpixel((row,col))
            (r1,g1,b1) = im.getpixel((row,col+1))
            
            s += struct.pack("B", (r0 & 0xF0) | (g0 >> 4))
            s += struct.pack("B", (b0 & 0xF0) | (r1 >> 4))
            s += struct.pack("B", (g1 & 0xF0) | (b1 >> 4))
    return s

if __name__=="__main__":
    import sys
    arduino = open_arduino()
    print convert_image_for_arduino(sys.argv[1])
    arduino.write(convert_image_for_arduino(sys.argv[1]))
