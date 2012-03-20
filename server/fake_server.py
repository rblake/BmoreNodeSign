#!/usr/bin/env python

import sys
import pygame
import os
import struct
import numpy as np

if __name__=="__main__":
    pixel_size = 8
    size = width, height = 8*16 , 8
    speed = [2, 2]

    black = 0, 0, 0
    white = 255, 255, 255
    screen = pygame.display.set_mode((width*pixel_size, height*pixel_size))
    small_screen = pygame.Surface((width, height))

    os.mkfifo("pipe")
    pipe = open("pipe", "rb", 0)
    

    data = ""
    while 1:
        #read in the buffer
        data += pipe.read()
        frame_extent = width*height*3
        if len(data) >= frame_extent:
            print "displaying", len(data)
            this_frame = np.reshape(np.fromstring(data[0:frame_extent], dtype=np.uint8, count=frame_extent), (width,height,3))
            data = data[frame_extent:]

            pygame.surfarray.blit_array(small_screen, this_frame)
            pygame.transform.scale(small_screen, screen.get_size(), screen)
            pygame.display.flip()
        
        for event in pygame.event.get():
            if event.type == pygame.QUIT: sys.exit()


