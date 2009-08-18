import wx
import yaml

class Object(object):

    BUOY = 1

    def __init__(self, type_, pos, rotation = 0, depth = 0):
        self._type = Object.getObjectType(type_)
        self._pos = pos
        self._rotation = rotation
        self._depth = depth

    def DrawObject(self, dc):
        """
        Implement this function so that the object will draw itself on the
        panel correctly.
        """
        pass

    @staticmethod
    def getObjectType(type_):
        # Python switch statement
        return {
            Object.BUOY : '[sim.vision.IBuoy, sim.vision.Buoy]'
            }

class Editor(wx.Panel):
    def __init__(self, parent, width, height, gridSize, gridOn = True,
                 style = wx.FULL_REPAINT_ON_RESIZE, *args, **kwargs):
        wx.Panel.__init__(self, parent, *args, **kwargs)

        self._width = width
        self._height = height
        self._gridSize = gridSize
        self._gridOn = gridOn

        self._objects = []

        self.LoadSceneFile('/home/jsternberg/ram_code/tools/simulator/data/scenes/nbrf_tank.sml')
        
        self.Bind(wx.EVT_PAINT, self.OnPaint)

    def AddObject(self, obj):
        self._objects.Append(obj)

    def EnableGrid(self, gridOn = True):
        self._gridOn = gridOn

    def DisableGrid(self):
        self.EnableGrid(gridOn = False)

    def LoadSceneFile(self, fileName):
        data = {}
        try:
            stream = file(fileName, 'r')
            data = yaml.load(stream)
            stream.close()
            print data
        except (IOError, yaml.YAMLError):
            # File does not exist, ignore
            pass

    def Resize(self, size):
        width, height = size
        #width = (width / self._width) * self._width

        # The 50 offset is for the status bar at the bottom
        #height = (height / self._height) * self._height - 50

        self.SetSize((width, height - 50))

    def OnPaint(self, event):
        # Preparation to start drawing
        dc = wx.PaintDC(self)
        dc.Clear()
        dc.BeginDrawing()

        # Draw the grid first if it is on
        if self._gridOn:
            winWidth, winHeight = self.GetSize()
            dc.SetPen(wx.Pen(wx.Colour(180, 180, 180), 1))

            # Draw the horizontal lines
            inc = winHeight / self._height
            y, yoffset = inc, winHeight % self._height
            for i in xrange(0, self._height):
                # This adds 1 as an offset for decimal numbers in the division
                if i <= yoffset:
                    y += 1
                dc.DrawLine(0, y, winWidth, y)
                y += inc

            # Draw the vertical lines
            inc = winWidth / self._width
            x, xoffset = inc, winWidth % self._width
            for i in xrange(0, self._width):
                # The offset
                if i <= xoffset:
                    x += 1
                dc.DrawLine(x, 0, x, winHeight)
                x += inc

        for obj in self._objects:
            obj.DrawObject(dc)

        dc.EndDrawing()

        del dc
