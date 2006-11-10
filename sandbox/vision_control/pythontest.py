from PIL import Image

def reset():
	im=Image.open("/Users/Dan/Desktop/DSC00099-1.jpg");
	im.thumbnail((128,128))
	return im

im=reset()
im.show()

def grayscale(im):
	(width,height)=im.size
	count=0
	print width
	print height
	for y in range(height):
		for x in range(width):
			count=count+1
			mask=0
			(r,g,b)=im.getpixel((x,y))
			(r,g,b)=(255-r,255-g,255-b)
			value=pow((r*r+g*g+b*b),.5)
			im.putpixel((x,y),(value,value,value))
	return im

def sobel(im,im2):
	GX=((-1,0,1),(-2,0,2),(-1,0,1))
	GY=((1,2,1),(0,0,0),(-1,-2,-1))
	
	data=im.getdata()
		
	for Y in range (1,data.size[1]-1):
		for X in range (1,data.size[0]-1):
			sumX = 0;
			sumY = 0;
			for I in range (-1,2):
				for J in range (-1,2):  
					sumX = sumX + (int)(((data[X+I + (Y+J)*data.size[0]])[0]) * GX[I+1][J+1])	

			for I in range (-1,2):
				for J in range (-1,2):  
					sumY = sumY + (int)(((data[X+I + (Y+J)*data.size[0]])[0]) * GY[I+1][J+1])

			SUM = abs(sumX) + abs(sumY);
	
			if(SUM>255):
				SUM=255
	        	if(SUM<0):
				SUM=0
			
			im2.putpixel((X,Y),(255-SUM,255-SUM,255-SUM))

def threshold(image,val):
        for x in range (image.size[0]):
                for y in range(image.size[1]):
                        if (image.getpixel((x,y))[0] < val):
                                image.putpixel((x,y),(0,0,0))
                        else:
                                image.putpixel((x,y),(255,255,255))

def explore(image,(i,j),color):
	count=0
	(width,height)=image.size
	if (image.getpixel((i,j))==(0,0,0)):
		image.putpixel((i,j),color)
		if (i-1 >= 0):
			count+=explore(image,(i-1,j),color)
		if (i+1 <width):
			count+=explore(image,(i+1,j),color)
		if (j-1 >= 0):
			count+=explore(image,(i,j-1),color)
		if (j+1 <height):
			count+=explore(image,(i,j+1),color)
		return count+1
	return 0
		
def DFS(image):
	(width,height)=image.size
	colors=((255,0,0),(0,255,0),(0,0,255),(255,0,0),(0,255,0),(0,0,255),(255,0,0),(0,255,0),(0,0,255),(255,0,0),(0,255,0),(0,0,255))
	count=[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]
	colorIndex=0;
	for y in range(height):
		for x in range(width):
			color=colors[colorIndex]
			if (image.getpixel((x,y))==(0,0,0)):
				count[colorIndex]=explore(image,(x,y),color)
				print count[colorIndex] 
				print " pixels are color \n"
				print color
				colorIndex=colorIndex+1
			
im=grayscale(im)
im.show()
im2=Image.new(im.mode,im.size)
im2.show()
sobel(im,im2)
im2.show()
threshold(im2,75)
im2.show()
DFS(im2)
im2.show()
