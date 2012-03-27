#!/usr/bin/env python

import sys
import pygame
import os
import struct
import numpy as np

class PixelDisplay:
    def __init__(self, width, height, pixel_size=8):
        self.pixel_size = pixel_size
        self.width = width
        self.height = height
        
        self.screen = pygame.display.set_mode((self.width*self.pixel_size, self.height*self.pixel_size))
        
        self.small_screen = pygame.Surface((self.width,self.height))
        

    def render_frame(self, data):
        this_frame = np.reshape(np.fromstring(data, dtype=np.uint8, count=self.width*self.height*3), (self.width,self.height,3))

        pygame.surfarray.blit_array(self.small_screen, this_frame)
        self.render_surface()

    def render_surface(self):
        pygame.transform.scale(self.small_screen, self.screen.get_size(), self.screen)
        pygame.display.flip()

    def get_surface(self):
        return self.small_screen


if __name__=="__main__":

    p = PixelDisplay(64,8,12)

    os.mkfifo("pipe")
    pipe = open("pipe", "rb", 0)
    
    data = ""
    while 1:
        #read in the buffer
        data += pipe.read()
        frame_extent = p.width*p.height*3
        if len(data) >= frame_extent:
            print "displaying", len(data)
            p.render_frame(data[0:frame_extent])
            data = data[frame_extent:]
        
        for event in pygame.event.get():
            if event.type == pygame.QUIT: sys.exit()


