from asyncio import ThreadedChildWatcher
import numpy as np
import pandas as pd
import cv2

# t = 0
ran0 = 0
ran1 = 998-938  # 998-938 # 808-677 # for left foot # 765-677 # for right foot # 0 for white_naive_dinosaur
#676,721
img = cv2.imread("download.png")
# print(len(img)) #1204*68
# retval, threshold1 = cv2.threshold(img, 83, 255, cv2.THRESH_BINARY)
retval, threshold2 = cv2.threshold(img, 83, 255, cv2.THRESH_BINARY)
## new_pic2 = threshold1[ran0+1:ran0+49,ran1+677:ran1+721]
# new_pic = threshold2[ran0+1:ran0+49,ran1+677:ran1+721]
# new_pic = threshold2[3:31,181:224]
# cv2.imshow('new2',new_pic)
new_pic = threshold2[ran0+20:ran0+48,ran1+938:ran1+998]
cv2.imshow('new2',new_pic)
# new_pic = cv2.resize(new_pic,(22,24),interpolation=cv2.INTER_CUBIC)

# new_pic = cv2.resize(new_pic,(33,16),interpolation=cv2.INTER_CUBIC) # for prone
# new_pic = cv2.resize(new_pic,(7,17),interpolation=cv2.INTER_CUBIC) # for cactus_small
# new_pic = cv2.resize(new_pic,(13,25),interpolation=cv2.INTER_CUBIC) # for cactus_big
# new_pic = cv2.resize(new_pic,(22,16),interpolation=cv2.INTER_CUBIC) # for bird
new_pic = cv2.resize(new_pic,(33,16),interpolation=cv2.INTER_CUBIC) # for prone
## new_pic2 = cv2.resize(new_pic2,(22,24),interpolation=cv2.INTER_CUBIC)
# cv2.imshow('new2',new_pic2)
retval, new_pic = cv2.threshold(new_pic, 80, 255, cv2.THRESH_BINARY)
cv2.imshow('new',new_pic)
a = np.zeros(dtype = int,shape=(len(new_pic),len(new_pic[0])))
# if new_pic[3][13][0] == 255:
#     new_pic[3][13][0] = 0
#     new_pic[3][13][1] = 0
#     new_pic[3][13][2] = 0
# if new_pic[4][24][0] == 255:
#     new_pic[4][24][0] = 0
#     new_pic[4][24][1] = 0
#     new_pic[4][24][2] = 0
# cv2.imshow('new',new_pic)
for y in range(len(new_pic)):
    for x in range(len(new_pic[y])):
        if new_pic[y][x][0] == 255:
            a[y][x] = 1
# if a[3][13] == 1:
#     a[3][13] = 0
# with open("dinosaur",'w') as d:
# with open("white_dinosaur",'w') as d:
with open("./source/not_full_prone_right_dinosaur",'w') as d:
    d.write("static u8 dinosaur_static[24][22] = ")
    d.write("{\n")
    for y in range(len(new_pic)):
        d.write("{")
        for x in range(len(new_pic[y])):
            d.write(str(a[y][x]))
            if x != len(new_pic[y])-1 : 
                d.write(', ')
                # d.write(', ')
        d.write("},\n")
        # d.write(",\n")
    d.write("};\n")
# print(a)

# cv2.imshow('original',img)
# cv2.imshow('threshold',threshold)

cv2.waitKey(0)
cv2.destroyAllWindows()

# img = cv2.imread("1.png")
# cv2.imshow('img',img)

# cv2.waitKey(0)
# cv2.destroyAllWindows()