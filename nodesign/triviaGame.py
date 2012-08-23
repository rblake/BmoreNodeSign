#!/usr/bin/env python

import pygame

from lightDisplay import LightDisplayClientFactory, d_wait
from twisted.internet import reactor, defer
from twisted.internet.endpoints import TCP4ClientEndpoint

def pixelize(l):
    return (l[0],l[1],l[2])

class TextRenderer:
    def __init__(self):
        self.font = pygame.image.load("8bitfont.png")
        font_width = self.font.get_width()
        array = pygame.surfarray.array3d(self.font)
        self._font_offset = {}
        offset=0
        for c in xrange(32,127):
            width = 1
            while offset+width < font_width and pixelize(array[offset+width,0,:]) != (255,0,0):
                width += 1
            self._font_offset[chr(c)] = (offset,width)
            offset += width

    def render_text_bitmap(self, text):
        string_width = len(text)-1
        for t in text:
            (offset,width) = self._font_offset[t]
            string_width += width
        rendered_text = pygame.Surface((string_width,8))
        cursor=0
        for t in text:
            (offset,width) = self._font_offset[t]
            rendered_text.blit(self.font, (cursor,0), area=pygame.Rect(offset,1,width,8))
            cursor += width+1

        return rendered_text

class Effects:
    def __init__(self, width, height):
        self.width = width
        self.height = height
        self.surface = pygame.Surface((width,height))

    def copy_display(self):
        return self.surface.copy()

    def fill_screen(self,color):
        frame = self.surface
        frame.fill(color)
        yield frame

    def marquee(self,bitmap,direction="left",begin="right",end="left"):
        bbbb = self.copy_display()
        if direction=="left":
            pos_increment = -1
        else:
            pos_increment = 1
        
        if begin=="left":
            begin_pos = 0
        else:
            begin_pos = bbbb.get_width()
        if end=="left":
            end_pos = -bitmap.get_width() 
        else:
            end_pos = bbbb.get_width()-bitmap.get_width()
            
        pos = begin_pos
        while 1:
            frame = bbbb.copy()
            frame.blit(bitmap, (pos,0))
            yield frame
            pos += pos_increment
            if pos*pos_increment >= end_pos*pos_increment:
                break



class TriviaQuestion:
    def __init__(self,question, answer, choices=[]):
        self.question = question
        self.answer = answer
        self.choices = choices


@defer.inlineCallbacks
def trivia_game(d_lights):
    lights = yield d_lights
    database = [TriviaQuestion("The answer is A", "A", ["Asadfsfdsd ", "Badsfsadfdsfs", "Casdfsadfdsf"]),
                TriviaQuestion("Pick 2", "2", ["1", "2", "3"]),
                ]
    tq_index = 0
    width, height = yield lights.d_size()
    background = (0,0,0)
    e = Effects(width,height)
    ttt = TextRenderer()
    while True:
        yield lights.d_acquire()
        for f in e.fill_screen(background): lights.d_display(f)
        tq = database[tq_index]
        for f in e.marquee(ttt.render_text_bitmap(tq.question), direction="left",begin="right",end="left"): lights.d_display(f)
        yield lights.d_waitForDisplay()
        for choice in tq.choices:
            for f in e.fill_screen(background): lights.d_display(f)
            first = 1
            for f in e.marquee(ttt.render_text_bitmap(choice),direction="left",begin="left",end="right"):
                lights.d_display(f)
                if first:
                    yield lights.d_waitForDisplay()
                    yield d_wait(0.5)
                first = 0
            yield lights.d_waitForDisplay()
            yield d_wait(0.5)
        first = 1
        for f in e.fill_screen(background): lights.d_display(f)
        for f in e.marquee(ttt.render_text_bitmap("Answer: "+tq.answer),direction="left",begin="left",end="left"):
            lights.d_display(f)
            if first:
                yield lights.d_waitForDisplay()
                yield d_wait(0.5)
            first = 0
        yield lights.d_waitForDisplay()
        yield d_wait(0.5)
        lights.release()
        tq_index += 1
        if tq_index >= len(database):
            tq_index = 0
        

if __name__=="__main__":
    point = TCP4ClientEndpoint(reactor, "localhost", 1234)
    trivia_game(point.connect(LightDisplayClientFactory()))
    reactor.run()
