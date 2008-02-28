# Based on C++ code by Thomas Monjalon
# Developed by Daniel Eloff on 14/9/07

import wx

def change_intensity(color, fac):
    rgb = [color.Red(), color.Green(), color.Blue()]
    for i, intensity in enumerate(rgb):
        rgb[i] = min(int(round(intensity*fac, 0)), 255)
        
    return wx.Color(*rgb)    

class LED(wx.Control):
    WIDTH = 17
    HEIGHT = 17
    
    def __init__(self, parent, id=-1, colors= None, pos=(-1,-1),
                 size = (WIDTH, HEIGHT), style=wx.NO_BORDER,
                 state = -1):
        if colors is None:
            colors = [wx.Colour(220, 10, 10),  # Red
                      wx.Colour(250, 200, 0),  # Yellow
                      wx.Colour(10, 220, 10),  # Green
                      wx.Colour(0, 0, 0)]      # Black
        
        wx.Control.__init__(self, parent, id, pos, size, style)
        self.MinSize = (LED.WIDTH, LED.HEIGHT)
        
        self.bmp = None
        self._colors = colors
        self._state = None
        self.Bind(wx.EVT_PAINT, self.OnPaint, self)

        self.SetState(state)
        
    def SetState(self, i):
        if i < 0:
            raise ValueError, 'Cannot have a negative state value.'
        elif i >= len(self._colors):
            raise IndexError, 'There is no state with an index of %d.' % i
        elif i == self._state:
            return

        self._state = i
        base_color = self._colors[i]
        light_color = change_intensity(base_color, 1.15)
        shadow_color = change_intensity(base_color, 1.07)
        highlight_color = change_intensity(base_color, 1.25)
        
        ascii_led = '''
        000000-----000000      
        0000---------0000
        000-----------000
        00-----XXX----=00
        0----XX**XXX-===0
        0---X***XXXXX===0
        ----X**XXXXXX====
        ---X**XXXXXXXX===
        ---XXXXXXXXXXX===
        ---XXXXXXXXXXX===
        ----XXXXXXXXX====
        0---XXXXXXXXX===0
        0---=XXXXXXX====0
        00=====XXX=====00
        000===========000
        0000=========0000
        000000=====000000
        '''.strip()
        
        xpm = ['17 17 5 1', # width height ncolors chars_per_pixel
               '0 c None', 
               'X c %s' % base_color.GetAsString(wx.C2S_HTML_SYNTAX).encode('ascii'),
               '- c %s' % light_color.GetAsString(wx.C2S_HTML_SYNTAX).encode('ascii'),
               '= c %s' % shadow_color.GetAsString(wx.C2S_HTML_SYNTAX).encode('ascii'),
               '* c %s' % highlight_color.GetAsString(wx.C2S_HTML_SYNTAX).encode('ascii')]
        
        xpm += [s.strip() for s in ascii_led.splitlines()]
        
        self.bmp = wx.BitmapFromXPMData(xpm)
        
        self.Refresh()
        
    def GetState(self):
        return self._state
    
    State = property(GetState, SetState)
    
    def OnPaint(self, e):
        width, height = self.GetSize()

        # Center the bitmap in the control
        x = int((width - LED.WIDTH) / 2.0)
        y = int((height - LED.HEIGHT) / 2.0)
        if x < 0:
            x = 0
        if y < 0:
            y = 0;
        
        dc = wx.PaintDC(self)
        if self.bmp is not None:
            dc.DrawBitmap(self.bmp, x, y, True)
