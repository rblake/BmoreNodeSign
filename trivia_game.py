#!/usr/bin/env python

import pygame
import time

from fake_server import PixelDisplay
from fake_server import TextRenderer

class TriviaQuestion:
    def __init__(self,question, answer, choices=[]):
        self.question = question
        self.answer = answer
        self.choices = choices

class Effects:
    def __init__(self, pixelDisplay):
        self.pd = pixelDisplay

    def render(self, frame):
        self.pd.get_surface().blit(frame,(0,0))
        self.pd.render_surface()
        
    def copy_display(self):
        return self.pd.get_surface().copy()

    def fill_screen(self,color):
        frame = self.copy_display()
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


if __name__=="__main__":
    import sys

    database = [TriviaQuestion("The answer is A", "A", ["Asadfsfdsd ", "Badsfsadfdsfs", "Casdfsadfdsf"]),
                TriviaQuestion("Pick 2", "2", ["1", "2", "3"]),
                ]


    ttt = TextRenderer()

    height=8
    width=8*8    
    p = PixelDisplay(width,height,12);
    background = (255,255,255)

    e = Effects(p)
    def display_func(tq, e, ttt=ttt):
        for f in e.fill_screen(background): yield f
        #get a random trivia question
        tq = database[tq_index]
        for f in e.marquee(ttt.render_text_bitmap(tq.question),direction="left",begin="right",end="left"): yield f
        for choice in tq.choices:
            for f in e.fill_screen(background): yield f
            first = 1
            for f in e.marquee(ttt.render_text_bitmap(choice),direction="left",begin="left",end="right"): 
                yield f
                if first:
                    time.sleep(0.5)
                first = 0
            time.sleep(0.5)
        first = 1
        for f in e.fill_screen(background): yield f 
        for f in e.marquee(ttt.render_text_bitmap("Answer: "+tq.answer),direction="left",begin="left",end="left"):
            yield f
            if first:
                time.sleep(0.5)
            first = 0
        time.sleep(0.5)

    tq_index = 0
    while 1:
        for frame in display_func(database[tq_index], e):
            e.render(frame)
            time.sleep(0.02)
        tq_index += 1
        if tq_index >= len(database):
            tq_index = 0
