from write_bitmap import *
import sys
from time import sleep

# Run "python animation.py <csv file> [speed]"
# For example, "python animation.py frames.csv .15"
# Right now the images will have to be in the same folder as 
# the python file. 

def main():
    frames = open(sys.argv[1], 'r').read()
    frames = frames.split(',')
    try: 
      speed = float(sys.argv[2])
    except:
      speed = .2

    arduino = open_arduino()

    frame = 0
    while(True):
        arduino.write(convert_image_for_arduino(frames[frame]))
        sleep(speed)
        frame = (frame + 1)%len(frames)
        print frame

if __name__=="__main__":
  main()
