#!/usr/bin/env python

import pygame
import time

from fake_server import PixelDisplay

class TextRenderer:
    def __init__(self):
        self.font = pygame.image.load("8x8font.png")
    
    def render_text_bitmap(self, text):
        rendered_text = pygame.Surface((len(text)*8,8))
        for ii in xrange(0,len(text)):
            rendered_text.blit(self.font, (ii*8,0), area=pygame.Rect(ord(text[ii])*8,0,8,8))
        return rendered_text


if __name__=="__main__":
    import sys
    text = " ".join(sys.argv[1:])

    ttt = TextRenderer()
    rendered_text = ttt.render_text_bitmap(text)

    pos = 0

    height=8
    width=8*8
    
    p = PixelDisplay(width,height,12);
    while 1:
        cols_rendered = 0
        text_pos= pos
        while cols_rendered < width:
            p.get_surface().blit(rendered_text, (cols_rendered, 0), pygame.Rect(text_pos,0,rendered_text.get_width()-text_pos,8))
            cols_rendered += rendered_text.get_width()-text_pos
            text_pos = 0
        
        p.render_surface()
        time.sleep(0.02)

        pos += 1
        if pos >= rendered_text.get_width():
            pos = 0

            
