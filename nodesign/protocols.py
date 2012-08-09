import collections

from twisted.protocols.stateful import StatefulProtocol
from twisted.internet.protocol import Protocol
from twisted.internet.task import LoopingCall
import write_image
import pygame
import numpy as np

class _BaseSignDriverProtocol(Protocol):
    def __init__(self, framerate, dims=(64, 8)):
        self.width, self.height = dims
        self._frame_buffer = collections.deque()
        self._looping_call = LoopingCall(self._display_next)
        self._looping_call.start(1.0 / framerate)

    def _display_next(self):
        if self._frame_buffer:
            self._display_now(self._frame_buffer.popleft())
        
    def dataReceived(self, frame):
        self._frame_buffer.append(frame)

class SignDriver(_BaseSignDriverProtocol):
    def __init__(self, framerate, dims=(64,8)):
        _BaseSignDriverProtocol.__init__(self, framerate, dims)
        
    def _display_now(self, frame):
        self.transport.write(write_image.convert_frame_for_arduino(frame))

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

