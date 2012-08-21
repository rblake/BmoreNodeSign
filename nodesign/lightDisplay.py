#!/usr/bin/env python

import os.path

from twisted.protocols.basic import NetstringReceiver
from twisted.internet.protocol import Protocol,Factory
from twisted.internet.task import LoopingCall
from twisted.internet import reactor, defer
from twisted.internet.task import deferLater

import collections
import pygame
import numpy as np
import write_image
import re

def d_wait(sec):
    #helper to wait for a number of seconds.
    return deferLater(reactor, sec, lambda: None)

_acquire_req = "GIMME THE DISPLAY\n"
_acquire_ack = "YOUR TURN\n"
_display_req = "DISPLAY\n"
_display_ack = "OK\n"
_release_req = "DONE WITH THE DISPLAY\n"

class LightDisplayClient(NetstringReceiver):
    def __init__(self):
        self._frameBuffer = collections.deque()
        self._accessQueue = collections.deque()

    def d_size(self):
        return self._sizeDeferred

    def d_acquire(self):
        d = defer.Deferred()
        self._accessQueue.append(d)
        self.sendString(_acquire_req)
        return d

    def d_waitForDisplay(self):
        return self._displayFinished

    def release(self):
        self.sendString(_release_req)

    def d_display(self, frame):
        if not self._frameBuffer:
            self._displayFinished = defer.Deferred()
        d = defer.Deferred()
        arr = pygame.surfarray.array3d(frame)
        stringified_frame = arr.tostring()
        self.sendString(_display_req+stringified_frame)
        self._frameBuffer.append(d)

    def connectionMade(self):
        NetstringReceiver.connectionMade(self)
        self._hasSize = False
        self._sizeDeferred = defer.Deferred()

    def stringReceived(self, frame):
        if not self._sizeDeferred.called:
            m = re.match(r'WIDTH (\d+) HEIGHT (\d+)\n', frame)
            if m:
                self._width = int(m.group(1))
                self._height = int(m.group(2))
                self._sizeDeferred.callback((self._width, self._height))
        elif frame==_acquire_ack:
            if self._accessQueue:
                self._accessQueue.popleft().callback(None)
        elif frame==_display_ack:
            if self._frameBuffer:
                self._frameBuffer.popleft().callback(None)
            if not self._frameBuffer:
                self._displayFinished.callback(None)
        else:
            #error!
            pass

class LightDisplayServer(NetstringReceiver):
    def __init__(self, lightDisplay):
        self._lightDisplay = lightDisplay

    def connectionMade(self):
        NetstringReceiver.connectionMade(self)
        self.sendString("WIDTH %d HEIGHT %d\n" % (self._lightDisplay.width, self._lightDisplay.height))

    def stringReceived(self, data):
        if data==_acquire_req:
            d = self._lightDisplay.d_acquire()
            d.addCallback(self.acquireCallback)
        elif data==_release_req:
            self._lightDisplay.release()
        elif data[0:len(_display_req)]==_display_req:
            frame = data[len(_display_req):]
            self._lightDisplay.d_display(frame).addCallback(self.displayCallback)

    def acquireCallback(self, result):
        self.sendString(_acquire_ack)

    def displayCallback(self, result):
        self.sendString(_display_ack)

class _BaseLightDisplay:
    def __init__(self, fps, dims):
        self.width, self.height = dims
        self._fps = fps
        self._frameBuffer = collections.deque()
        self._accessQueue = collections.deque()
        self._loopingCall = LoopingCall(self._displayNext)
        self._loopingCall.start(1.0/ self._fps)

    def d_acquire(self):
        d = defer.Deferred()
        self._accessQueue.append(d)
        if len(self._accessQueue)==1:
            d.callback(None)
        return d

    def release(self):
        self._accessQueue.popleft()
        if self._accessQueue:
            self._displayFinished.chainDeferred(self._accessQueue[0])

    def _displayNext(self):
        if self._frameBuffer:
            #display the client frame.
            frame, d = self._frameBuffer.popleft()
            self._displayNow(frame)
            d.callback(None)
            if not self._frameBuffer:
                self._displayFinished.callback(None)

    def d_display(self, frame):
        if not self._frameBuffer:
            self._displayFinished = defer.Deferred()
        d = defer.Deferred()
        self._frameBuffer.append((frame, d))
        return d

class InMemoryDisplay(_BaseLightDisplay):
    def __init__(self, pixelSize, fps, dims=(64,8)):
        _BaseLightDisplay.__init__(self, fps, dims)
        self.screen = pygame.display.set_mode(
            (self.width * pixelSize, self.height * pixelSize)
        )
        self.smallScreen = pygame.Surface((self.width,self.height))
        
    def _displayNow(self, frame):
        thisFrame = np.reshape(
            np.fromstring(
                frame,
                dtype=np.uint8,
                count=self.width * self.height * 3),
            (self.width, self.height, 3)
        )

        pygame.surfarray.blit_array(self.smallScreen, thisFrame)
        pygame.transform.scale(
            self.smallScreen,
            self.screen.get_size(),
            self.screen
        )
        pygame.display.flip()
        

class SingleSignDisplay(_BaseLightDisplay,Protocol):
    def __init__(self, fps, dims=(64,8)):
        _BaseLightDisplay.__init__(self, fps, dims)
        Protocol.__init__(self)
        serial_port = write_image.find_arduino()
        SerialPort(USBClient(), serial_port, reactor, baudrate='19200')

    def _displayNow(self, frame):
        self.transport.write(write_image.convert_frame_for_arduino(frame))


class LightDisplayServerFactory(Factory):
    def __init__(self, lightDisplay):
        self._lightDisplay = lightDisplay

    def buildProtocol(self, addr):
        return LightDisplayServer(self._lightDisplay)

class LightDisplayClientFactory(Factory):
    def buildProtocol(self, addr):
        return LightDisplayClient()

if __name__=="__main__":
    lightDisplay = InMemoryDisplay(16,30)
    reactor.listenTCP(1234, LightDisplayServerFactory(lightDisplay))
    reactor.run()
