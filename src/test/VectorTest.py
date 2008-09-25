# -*- coding: utf-8 -*-
import unittest

import sys, time, platform

# Import the correct version of libavg. Since it should be possible to
# run the tests without installing libavg, we add the location of the 
# uninstalled libavg to the path.
# TODO: This is a mess. 
sys.path += ['../wrapper/.libs', '../python']
if platform.system() == 'Darwin':
    sys.path += ['../..']     # Location of libavg in a mac installation. 

if platform.system() == 'Windows':
    from libavg import avg    # Under windows, there is no uninstalled version.
else:    
    import avg

from testcase import *

class VectorTestCase(AVGTestCase):
    def __init__(self, testFuncName):
        AVGTestCase.__init__(self, testFuncName, 24)

    def testLine(self):
        def addLines():
            canvas = Player.getElementByID("canvas")
            line = Player.createNode("line", {"x1":2, "y1":2, "x2":10, "y2":2})
            canvas.appendChild(line)
        Player.loadString("""
            <?xml version="1.0"?>
            <!DOCTYPE avg SYSTEM "../../doc/avg.dtd">
            <avg width="160" height="120">
              <canvas id="canvas" width="160" height="120"/>
            </avg>
        """)
        self.start(None,
                (addLines,
                 lambda: self.compareImage("testline", False), 
                ))

def vectorTestSuite():
    suite = unittest.TestSuite()
    suite.addTest(VectorTestCase("testLine"))
    return suite

Player = avg.Player.get()
