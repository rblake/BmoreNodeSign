#!/usr/bin/env python

import pygame

from lightDisplay import LightDisplayClientFactory, d_wait
from twisted.internet import reactor, defer
from twisted.internet.endpoints import TCP4ClientEndpoint

@defer.inlineCallbacks
def singleFrame(d_lights, image):
    lights = yield d_lights
    width, height = yield lights.d_size()
    yield lights.d_acquire()
    f = pygame.image.load(image)
    yield lights.d_display(f)
    lights.release()
    reactor.stop()

if __name__=="__main__":
    import sys

    point = TCP4ClientEndpoint(reactor, "localhost", 1234)
    singleFrame(point.connect(LightDisplayClientFactory()), sys.argv[1])
    reactor.run()
