#!/usr/bin/env python

from twisted.internet import reactor, defer
from lightDisplay import SingleSignDisplay,LightDisplayServerFactory

if __name__=="__main__":
    lightDisplay = SingleSignDisplay(10)
    reactor.listenTCP(1234, LightDisplayServerFactory(lightDisplay))
    reactor.run()
