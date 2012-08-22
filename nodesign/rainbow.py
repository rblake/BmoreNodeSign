#!/usr/bin/env python

import pygame
import colorsys
import numpy as np
import math

from lightDisplay import LightDisplayClientFactory, d_wait
from twisted.internet import reactor, defer
from twisted.internet.endpoints import TCP4ClientEndpoint

@defer.inlineCallbacks
def rainbow(d_lights):
    lights = yield d_lights
    tq_index = 0
    width, height = yield lights.d_size()
    surf = pygame.Surface((width,height))
    pixels = np.zeros([width,height,3], np.uint8)
    while True:
        #start rendering a cycle.
        yield lights.d_acquire()
        for p in xrange(0, 360*5, 5):
            for x in xrange(0, width):
                for y in xrange(0, height):
                    p_rad = p/180.*3.14159
                    hue = (2+math.sin(float(x)/width+p_rad+0.5)+math.sin((float(y)/height*1.2+p_rad)*1.2))/4
                    (r,g,b) = colorsys.hsv_to_rgb(hue,1,1)
                    pixels[x,y,0] = int(255*r)
                    pixels[x,y,1] = int(255*g)
                    pixels[x,y,2] = int(255*b)
            pygame.surfarray.blit_array(surf,pixels)
            lights.d_display(surf)
        lights.release()        

if __name__=="__main__":
    point = TCP4ClientEndpoint(reactor, "localhost", 1234)
    rainbow(point.connect(LightDisplayClientFactory()))
    reactor.run()
