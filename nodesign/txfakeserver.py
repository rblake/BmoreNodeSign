import os.path

from twisted.internet import protocol, reactor
from twisted.internet.abstract import FileDescriptor
from twisted.internet import fdesc

import protocols

class SignFactory(protocol.Factory):
    def buildProtocol(self, addr):
        return protocols.SignDriver(30)

    def setReactorOutput(self, reactor):
        serial_port = write_image.find_arduino()
        SerialPort(USBClient(), serial_port, reactor, baudrate='19200')

class InMemorySignFactory(protocol.Factory):
    def buildProtocol(self, addr):
        return protocols.InMemorySignDriver(30, 16)

    def setReactorOutput(self, reactor):
        pass

if __name__=="__main__":
    factory = InMemorySignFactory()
    reactor.listenTCP(1234, factory)
    factory.setReactorOutput(reactor)
    reactor.run()
