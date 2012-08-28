#!/usr/bin/env python

import pygame

from lightDisplay import LightDisplayClientFactory, d_wait
from twisted.internet import reactor, defer
from twisted.internet.endpoints import TCP4ClientEndpoint

@defer.inlineCallbacks
def loopFrames(d_lights, images):
    lights = yield d_lights
    width, height = yield lights.d_size()
    while True:
        for image in images:
            yield lights.d_acquire()
            f = pygame.image.load(image)
            yield lights.d_display(f)
            yield d_wait(3)
            lights.release()

if __name__=="__main__":
    import sys

    point = TCP4ClientEndpoint(reactor, "localhost", 1234)
    loopFrames(point.connect(LightDisplayClientFactory()), sys.argv[1:])
    reactor.run()
