import re
import math
boundingBox = []

class Node:
    def __init__(self, name, pos, width, height):
        self._name = name
        pos = pos.split(",")
        self._x = float(pos[0]) # x
        self._y = float(pos[1]) # y
        self._width = float(width)
        self._height = float(height)
        self.isCurrentState = False
    ''' Converts from dot coordiantes to wx coords given a window size '''
    #@staticmethod?
    def convert(self, windowX, windowY):
        self._y = self._y * 1.37
        self._x = self._x * 1.26
        
    def __str__(self):
        return self._name
            

class Edge:
    def __init__(self, label, path, labelPos):
        self._label = label
        tmpPath = path.split(" ")
        self._path = []
        for val in tmpPath:
            pair = val.split(",")
            if (len(pair) > 1):
                self._path.append([int(pair[0]),int(pair[1])])
            #else:
            #    self._path.append(int(val))
        tmp = labelPos.split(",")
        self._labelPos = map(int,tmp)
        #Make an iterator?
    
    def __str__(self):
        return self._label
        
input = '''digraph aistate {
node [label="\N"];
graph [bb="0,0,224,300"];
Searching [pos="78,282", width="1.14", height="0.50"];
Seek [pos="78,194", width="0.75", height="0.50"];
Hit [pos="78,106", width="0.75", height="0.50"];
End [pos="78,18", width="0.75", height="0.50"];
Searching -> Seek [label=LIGHT_FOUND, pos="e,54,203 47,270 36,264 25,256 19,246 10,230 27,217 45,207", lp="71,238"];
Seek -> Searching [label=LIGHT_LOST, pos="e,100,267 98,207 110,216 123,231 117,246 115,251 111,256 107,260", lp="160,238"];
Seek -> Hit [label=LIGHT_ALMOST_HIT, pos="e,78,124 78,176 78,164 78,148 78,134", lp="146,150"];
Hit -> End [label=FORWARD_DONE, pos="e,78,36 78,88 78,76 78,60 78,46", lp="136,62"];
}'''

input = '''
digraph aistate {
node [label="\N"];
graph [bb="0,0,265,300"];
Searching [pos="128,282", width="1.36", height="0.50"];
Seek [pos="49,194", width="0.81", height="0.50"];
Hit [pos="49,106", width="0.75", height="0.50"];
End [pos="32,18", width="0.75", height="0.50"];
Cool [pos="199,18", width="0.81", height="0.50"];
Searching -> Seek [label=LIGHT_FOUND, pos="e,28,207 84,274 54,268 17,258 8,246 0,235 8,223 20,213", lp="60,238"];
Seek -> Searching [label=LIGHT_LOST, pos="e,123,264 73,205 84,211 97,219 106,230 112,237 116,246 120,254", lp="157,238"];
Seek -> Hit [label=LIGHT_ALMOST_HIT, pos="e,49,124 49,176 49,164 49,148 49,134", lp="117,150"];
Hit -> End [label=FORWARD_DONE, pos="e,31,36 40,89 37,83 34,76 33,70 32,62 31,54 31,46", lp="94,62"];
Hit -> Cool [label=DONE, pos="e,186,35 75,100 96,95 127,85 150,70 161,62 171,52 179,43", lp="185,62"];
Cool -> Searching [label=LONG, pos="e,168,271 205,36 219,77 246,179 203,246 197,255 187,262 177,267", lp="245,150"];
}
'''


# find Searching [pos]
#pattern = re.compile('(\w+)\[pos="(\d+,\d+)", width="([0-9.]+), height="([0-9.]+)"\];')

pattern = re.compile('(\w+) \[pos="(\d+,\d+)", width="([0-9.]+)", height="([0-9.]+)"\];')
pattern2 = re.compile('(\w+) -> (\w+) \[label=(\w+), pos="e,([^"]+)", lp="(\d+,\d+)"\];') #fixup
patternBB = re.compile('graph \[bb="([^"]+)"\];')

#Label: LIGHT_ALMOST_HIT Pos: 78,124 78,176 78,164 78,148 78,134 LP: 146,150 From: Hit To: End -- 1.26X 1.37Y appx
nodes = {} 
edges = []
foundBB = False

for line in input.splitlines():
    # Search only once for the bounding box
    if not foundBB:
        bBox = patternBB.match(line)
        if bBox != None:
            tmpBB = bBox.group(1)
            tmpArr = tmpBB.split(",")
            boundingBox = map(int, tmpArr)
            foundBB = True
    result = pattern.match(line)
    if result != None:
        name = result.group(1)
        pos = result.group(2)
        width = result.group(3)
        height = result.group(4)
        nodes[name] = Node(name,pos,width,height)
    else: # try other pattern
        result = pattern2.match(line)
        if result != None:
            #print "From:", result.group(1), "To:",result.group(2)
            label = result.group(3)
            path = result.group(4) 
            labelPos =result.group(5)
            edges.append(Edge(label,path,labelPos))
#----------------------------------------------------------------------

import wx
import wx.aui
class PanelDrawer(wx.PyControl):
    def __init__(self, parent, *args, **kwargs):
        wx.PyControl.__init__(self, parent, *args, **kwargs)
        self.Bind(wx.EVT_PAINT, self.OnPaint)

    
    def OnPaint(self, event):
        dc = wx.PaintDC(self)
        gc = wx.GraphicsContext.Create(dc)
        self.Draw(gc,dc)
        
    #void DrawEllipse(wxDouble x, wxDouble y, wxDouble w, wxDouble h)
    """
    void AddCurveToPoint(wxDouble cx1, wxDouble cy1, wxDouble cx2, wxDouble cy2, wxDouble x, wxDouble y)
    Adds a cubic Bezier curve from the current point, using two control points and an end point.
    void AddCurveToPoint(const wxPoint2DDouble& c1, const wxPoint2DDouble& c2, const wxPoint2DDouble& e)
    """
    def Draw(self,gc,dc):
        width,height = self.GetClientSize()
    
        gc.SetPen(wx.BLACK_PEN)
        for label in nodes:
            node = nodes[label]
            w = node._width * 72
            h = node._height * 72
            y = height - node._y - (h/2.0)
            x = node._x - (w/2.0)
            
            ellipse = gc.CreatePath()
            ellipse.AddEllipse(x,y,w,h)

            if node.isCurrentState == True:
                gc.SetBrush(wx.RED_BRUSH)
                gc.FillPath(ellipse)
                gc.SetBrush(wx.TRANSPARENT_BRUSH)
            else:
                gc.DrawPath(ellipse)
            
            self.drawNodeText(gc, dc, node._name, x, y, w , h )
            #print w , "W" , h , "H"
        cSize = 3
        for edge in edges:
            if (len(edge._path) == 8):
                x1,y1 = edge._path[0]
                x2,y2 = edge._path[1]
                x3,y3 = edge._path[3]

                # Invert from dot coords to our coords
                y1 = height - y1 
                y2 = height - y2 
                y3 = height - y3        
                curvePath = gc.CreatePath()
                curvePath.MoveToPoint([x1,y1])
                curvePath.AddCurveToPoint([x1,y1],[x3,y3],[x2,y2])
                self.drawEdgeLabel(gc, dc, edge._label, x3, y3)
                gc.StrokePath(curvePath)
                circlePath = gc.CreatePath()
                circlePath.AddCircle(x1,y1,cSize)
                gc.SetBrush(wx.BLACK_BRUSH)
                gc.DrawPath(circlePath)
                
                #if x1 <= x2 and y2 < y1: #more or less, straight down
                #    trianglePath = gc.CreatePath()
                #    trianglePath.MoveToPoint([x1,y1])
                #    trianglePath.AddLineToPoint([x1-tSize,y1-tSize])
                #    trianglePath.AddLineToPoint([x1+tSize,y1-tSize])
                #    trianglePath.AddLineToPoint([x1,y1])
                #    gc.SetBrush(wx.BLACK_BRUSH)
                #    gc.FillPath(trianglePath)
                #elif y2 > y1: #angled arrow
                   
            else:
                x1,y1 = edge._path[0]
                x2,y2 = edge._path[1]
                # Invert from dot coords to our coords
                y1 = height - y1 
                y2 = height - y2         
                curvePath = gc.CreatePath()
                curvePath.MoveToPoint([x1,y1])
                curvePath.AddLineToPoint([x2,y2])
                gc.StrokePath(curvePath)

                # Draw Triangle
                #trianglePath = gc.CreatePath()
                dist = y2-y1
                """ Use label position """
                self.drawEdgeLabel(gc, dc, edge._label, x1, y1 + (dist/1.5))
                """ Draw circle to show direction """
                circlePath = gc.CreatePath()
                circlePath.AddCircle(x1,y1,cSize)
                gc.SetBrush(wx.BLACK_BRUSH)
                gc.DrawPath(circlePath)
                """ Draw the triangle that shows  edge's direction """
                #trianglePath = gc.CreatePath()
                #trianglePath.MoveToPoint([x1,y1])
                #tSize = 5
                #trianglePath.AddLineToPoint([x1-tSize,y1-tSize])
                #trianglePath.AddLineToPoint([x1+tSize,y1-tSize])
                #trianglePath.AddLineToPoint([x1,y1])
                #gc.SetBrush(wx.BLACK_BRUSH)
                #gc.FillPath(trianglePath)

        self.Refresh()
        
    
    def drawEdgeLabel(self, gc, dc, label, x, y):
        font = wx.SystemSettings.GetFont(wx.SYS_DEFAULT_GUI_FONT)
        font.SetWeight(wx.BOLD) 
        font.SetPointSize(int(float(font.GetPointSize()))) 
        textWidth, textHeight =dc.GetTextExtent(label)
        gc.SetFont(font)
        gc.DrawText(label, x , y )

    ''' Modified version of Joe`s method '''
    def drawNodeText(self, gc, dc, name, x, y, w ,h):
        """
        Draws the currrent rotation in the middle of the circle
        """       
        # Set Font
        font = wx.SystemSettings.GetFont(wx.SYS_DEFAULT_GUI_FONT)
        font.SetWeight(wx.BOLD) 
        font.SetPointSize(int(float(font.GetPointSize()) * 1.2)) 
        textWidth, textHeight =dc.GetTextExtent(name)
 
        gc.SetFont(font)
        gc.DrawText(name, x + (w / 2.0) - (textWidth / 2.0) , y + (h / 2.0) - (textHeight / 2.0))
import random            
class MainWindow(wx.Frame):
    def __init__(self,parent,id,title):
        wx.Frame.__init__(self,parent,wx.ID_ANY, title, size = (600,500))
        sizer = wx.BoxSizer(wx.VERTICAL)
        p = PanelDrawer(self)
        btn = wx.Button(self, id=-1, label='Random', size=(70, 28))
        sizer.Add(p,1, wx.EXPAND | wx.ALL,5)
        sizer.Add(btn)
        btn.Bind(wx.EVT_BUTTON, self.btnNext)
        self.SetAutoLayout(True)
        self.SetSizer(sizer)
        self.Show(True)
        
    def btnNext(self,e):
        r = random.randint(1, len(nodes))
        count = 0
        for label in nodes:
            node = nodes[label]
            count += 1
            if r == count:
                node.isCurrentState = True
            else:
                node.isCurrentState = False
        self.Refresh() 

app = wx.PySimpleApp()
frame = MainWindow(None, -1, "state graph drawing tester")
app.MainLoop()