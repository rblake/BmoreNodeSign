#!/usr/bin/env python

import subprocess
import re
import pygame

from lightDisplay import LightDisplayClientFactory, d_wait
from twisted.internet import reactor, defer
from twisted.internet.endpoints import TCP4ClientEndpoint

from triviaGame import TextRenderer,Effects


def get_output(command):
    process = subprocess.Popen(command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, universal_newlines=True)
    output,unused = process.communicate()
    retcode = process.poll()
    if retcode:
            raise subprocess.CalledProcessError(retcode, command, output=output[0])
    return output

def clean_for_display(text):
    cleaned = text
    cleaned = re.sub(r'[ \t\f\v]+',' ',cleaned)
    cleaned = re.sub(r'\r?\n','   ',cleaned)
    return cleaned


@defer.inlineCallbacks
def displayFortune(d_lights):
    lights = yield d_lights
    width, height = yield lights.d_size()
    e = Effects(width,height)
    ttt = TextRenderer()
    background = (0,0,0)
    while True:
        fortune = clean_for_display(get_output("fortune"))
        text_bitmap = ttt.render_text_bitmap(fortune)
        yield lights.d_acquire()
        for f in e.marquee(text_bitmap, direction="left",begin="right",end="left"): 
            lights.d_display(f)
            yield d_wait(0)
        lights.release()

if __name__=="__main__":
    import sys

    point = TCP4ClientEndpoint(reactor, "localhost", 1234)
    displayFortune(point.connect(LightDisplayClientFactory()))
    reactor.run()
