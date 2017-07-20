from PIL import Image
im = Image.open("testy.mjpeg")
pix = im.load()

X,Y = im.size

avg_x = 0
hits = 0

red = ""
green = ""
blue = ""

for x in range(X):
    for y in range(Y):
        if sum(pix[x,y])> 240*3 :
            #pix[x,y] = (0,0,0)
        
            avg_x += x
            hits += 1
        r,b,g = pix[x,y]
        red += "%d," %r
        blue += "%d,"% g
        green += "%d,"%b
    red +="\n"
    blue +="\n"
    green +="\n"

with open("center_red_jpeg.csv","wb")as f:
    f.write(red)


with open("center_blue_jpeg.csv","wb")as f:
    f.write(blue)

with open("center_green_jpeg.csv","wb")as f:
    f.write(green)



if hits > 0:
    avg_x =avg_x/hits
    if avg_x < X/2:
        direction = "LEFT"
    else:
        direction = "RIGHT"

    print "\n\n\n\n **INSTRUCTIONS \n\n\naverage x is %s.  Go %s" % (avg_x, direction)

else:
    print "location not found!"
