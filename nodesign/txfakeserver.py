import os.path

from twisted.internet import reactor
from twisted.internet.abstract import FileDescriptor
from twisted.internet import fdesc

from nodesign import protocols

class NamedPipeReader(FileDescriptor):
    def __init__(self, filename, protocol):
        FileDescriptor.__init__(self)
        self.fileno = os.open(filename, os.O_RDONLY)
        fdesc.setNonBlocking(self.fileno)
        self.protocol = protocol
        self.protocol.makeConnection(self)

    def doRead(self):
        buf = os.read(self.fileno, 4096)
        if buf:
            self.protocol.dataReceived(buf)
        else:
            self.protocol.connectionLost()

    def loseConnection(self):
        os.close(self.fileno)

def main():
    p = protocols.SignProtocol(dims=(64, 16))
    f = NamedPipeReader(
        os.path.join(os.path.expanduser("~"), "nodesign"),
        protocols.InMemorySignDriver(30, 1)
    )

    reactor.addReader(f)
    reactor.run()

main()
