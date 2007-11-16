import wx
class ThrusterBar(wx.PyControl):
    def __init__(self, parent, innerBorder=0, id=wx.ID_ANY, pos=wx.DefaultPosition,
                 size=wx.DefaultSize, style=wx.NO_BORDER, validator=wx.DefaultValidator,name="valbar"):
        wx.PyControl.__init__(self, parent, id, pos, size, style, validator, name)
        self.parent = parent
        """ Default to 0% """
        self.barValue = 50 
        """ The max/min values of the bar """
        self.maxValue = 100
        self.minValue = -100
        
        self.innerBorder=innerBorder 

        self.Bind(wx.EVT_PAINT, self.OnPaint)
        self.Bind(wx.EVT_ERASE_BACKGROUND, self.OnEraseBackground)

    def increment(self, step=1):
        self.setVal(self.barValue+step)
    
    def decrement(self, step=1):
        self.setVal(self.barValue-step)
        
    def setVal(self,val):
        if val <= self.maxValue and val >= self.minValue:
            self.barValue=val
        else:
            return None 
            """return None if we could not process setVal due to constraints"""
        
    def OnPaint(self, event):
        """ Handles the wx.EVT_PAINT event for CustomCheckBox. """
        dc = wx.PaintDC(self)
        gc = wx.GraphicsContext.Create(dc)
        self.Draw(gc)
       
    def Draw(self,gc):
        width,height = self.GetSize()
        """ Draw the rectangle to represent the value """
        pen = wx.Pen("green", 1)
        brush = wx.Brush("green")
        if self.barValue > 0:
            gc.SetPen(pen)
            gc.SetBrush(brush)
        else:
            pen.SetColour("red")
            brush.SetColour("red")
            gc.SetPen(pen)
            gc.SetBrush(brush)
            
        path = gc.CreatePath()
        percent = float(self.barValue) / (self.maxValue*2) 
        heightScale = -height*percent
        
        path.AddRectangle(0+self.innerBorder, height/2, width-(self.innerBorder*2), heightScale) #innerBorder
        gc.DrawPath(path)
        
        pen.SetColour("black")
        pen.SetWidth(2)
        
        gc.SetPen(pen)
        
        gc.StrokeLine(0,height/2,width,height/2)
        
        brush.SetStyle(wx.TRANSPARENT)
        gc.SetBrush(brush)
        border = gc.CreatePath()
        border.AddRectangle(0, 0, width, height)
        gc.DrawPath(border)      
             

    def OnEraseBackground(self, event):
        """ Handles the wx.EVT_ERASE_BACKGROUND event. """
        pass