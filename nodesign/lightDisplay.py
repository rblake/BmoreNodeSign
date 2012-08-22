#!/usr/bin/env python

import os.path

from twisted.internet.serialport import SerialPort
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
        self._displayFinished = defer.succeed(None)

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
        return d

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
        self._displayFinished = defer.succeed(None)
        self._readyForNextDisplay = defer.succeed(None)

    def d_acquire(self):
        d = defer.Deferred()
        self._accessQueue.append(d)
        if len(self._accessQueue)==1:
            self._displayFinished.chainDeferred(self._accessQueue[0])
        return d

    def release(self):
        self._accessQueue.popleft()
        if self._accessQueue:
            self._displayFinished.chainDeferred(self._accessQueue[0])

    def d_display(self, frame):
        d = defer.Deferred()
        self._frameBuffer.append((frame, d))
        if len(self._frameBuffer) == 1:
            self._displayFinished = defer.Deferred()
            self._readyForNextDisplay.addCallback(self._displayNext)
        return d

    def _displayNext(self, unused):
        d_pause = self.d_minWaitBetweenFrames()
        frame, d = self._frameBuffer.popleft()
        d_display = self._d_displayNow(frame)
        d_display.chainDeferred(d)
        d_display.addCallback(self.finishDisplay)
        
        self._readyForNextDisplay = defer.gatherResults([d_pause,d_display])
        if self._frameBuffer:
            self._readyForNextDisplay.addCallback(self._displayNext)

    def d_minWaitBetweenFrames(self):
        d = d_wait(1.0/self._fps)
        return d

    def finishDisplay(self, unused):
        if not self._frameBuffer:
            self._displayFinished.callback(None)


class InMemoryDisplay(_BaseLightDisplay):
    def __init__(self, pixelSize, fps, dims=(64,8)):
        _BaseLightDisplay.__init__(self, fps, dims)
        self.screen = pygame.display.set_mode(
            (self.width * pixelSize, self.height * pixelSize)
        )
        self.smallScreen = pygame.Surface((self.width,self.height))

    def _d_displayNow(self, frame):
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
        return defer.succeed(None)

class SingleSignDisplay(_BaseLightDisplay):
    def __init__(self, fps, dims=(64,8)):
        _BaseLightDisplay.__init__(self, fps, dims)
        #serial_port = write_image.find_arduino()
        serial_port = "/dev/ttyUSB0"
        self.arduino = ArduinoProtocol(serial_port)

    def _d_displayNow(self, frame):
        return self.arduino.d_sendData(write_image.convert_frame_for_arduino(frame))

class ArduinoProtocol(Protocol):
    def __init__(self, serial_port):
        self.d_myName = defer.Deferred()
        SerialPort(self, serial_port, reactor, baudrate='115200')
        self._frameQueue = collections.deque()

    def dataReceived(self, data):
        m = re.match(r'(\d+)',data)
        if m:
            self.d_myName.callback(int(m.group(1)))
        elif data[0:2] == "OK":
            if self._frameQueue:
                d = self._frameQueue.popleft()
                d.callback(None)

    def d_sendData(self, data):
        d = defer.Deferred()
        self._frameQueue.append(d)
        self.transport.write(data)
        return d

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
