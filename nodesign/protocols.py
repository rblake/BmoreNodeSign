import collections

from twisted.protocols.stateful import StatefulProtocol
from twisted.internet.protocol import Protocol
from twisted.internet.task import LoopingCall
import pygame
import numpy as np

class SignProtocol(StatefulProtocol):
    alignment = "OK"

    def __init__(self, dims=(64, 16)):
        x, y = dims
        self.frame_size = x * y + len(self.alignment)
        self._last_char = "" # To be used for realigning

    def get_frame(self, msg):
        data = msg[:-2]
        check = msg[-2:]
        if check != self.alignment:
            return self._realign(), 1
        #actually display the frame

    def _realign(self, msg):
        if self._last_char + msg == self.alignment:
            self._last_char = ""
            return self.get_frame, self.frame_size
        self._last_char = msg
        return self._realign, 1
    
    def getInitialState(self):
        return self.get_frame, self.frame_size

    def send_frame(self, frame):
        self.transport.write(frame)
        self.transport.write(self.alignment)

class _BaseSignDriverProtocol(Protocol):
    alignment = "OK"

    def __init__(self, framerate, dims=(64, 8)):
        self.width, self.height = dims
        self._frame_buffer = collections.deque()
        self._looping_call = LoopingCall(self._display_next)
        self._looping_call.start(1.0 / framerate)

    def _display_next(self):
        if self._frame_buffer:
            self._display_now(self._frame_buffer.popleft())
        
    def send_frame(self, frame):
        self.frame_buffer.append(frame)

class SignDriver(_BaseSignDriverProtocol):
    def _display_now(self, frame):
        self.transport.writeSequence([frame, self.alignment])

class InMemorySignDriver(_BaseSignDriverProtocol):
    def __init__(self, framerate, pixel_size, dims=(64, 8)):
        _BaseSignDriverProtocol.__init__(self, framerate, dims)
        self.screen = pygame.display.set_mode(
            (self.width * pixel_size, self.height * pixel_size)
        )
        self.small_screen = pygame.Surface((self.width,self.height))

    def _display_now(self, frame):
        this_frame = np.reshape(
            np.fromstring(
                frame,
                dtype=np.uint8,
                count=self.width * self.height * 3),
            (self.width, self.height, 3)
        )

        pygame.surfarray.blit_array(self.small_screen, this_frame)
        self.render_surface()

    def render_surface(self):
        pygame.transform.scale(
            self.small_screen,
            self.screen.get_size(),
            self.screen
        )
        pygame.display.flip()

