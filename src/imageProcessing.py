'''
Last updated on 2 June 2014
Added functionality regarding the mean- used to detect if something really is an image
@bug: x * avg might serve to be based on the relative avg (higher -> more selective)
@author: Jess Smith
'''

import cv2
import numpy as np
import matplotlib.pyplot as plt
print('imports')
        
#method for preprocessing; converts a given image to grayscale and applies a threshold
#returns a tuple of img, ret, and thresh            
def preProcessing(img):
    if (len(img.shape) == 3) or (len(img.shape) == 4):
        img = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
        
    tempAvg = np.average(img)

    ret, thresh = cv2.threshold(img, 120, 255, 0) #normally at 120, 255, 0
    avg = np.average(thresh)
    
    print("temp: " + str(tempAvg))
    print("avg: " + str(avg))

    return img, ret, thresh, avg, tempAvg

#takes a grayscale image and the thresh one
#produces the contours and draws them on imgray
def analyzeContours(imgray, thresh, avg, tempAvg):
    rez = []
    counter = 0
    
    contours, hierarchy = cv2.findContours(thresh, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)    
    imArea = thresh.shape[0] * thresh.shape[1]    
    
    for h,cnt in enumerate(contours):
        x,y,w,h = cv2.boundingRect(cnt)
        currArea = float(w) * h
        ratio = currArea / imArea 
        
        '''
        # if the rectangle is less than 95% of the image area
        # and if it its greater than 0.1% of it
        # and if its pixel whiteness is less than 60% of the avg:
        # this is used to filter out extremely small contours and one that 
            # is for the image overall
            # also, this filters out things such as the insides of 0's (low intensities)
        
        #maybe, if the avg intensity is really high, make the discerning avg lower?
            
        # TODO: add functionality to check sub-images '''
        
        # this process picks up too many images 
        # RATIO ADJUSTABLE *AA**A*A*A*A*A*A*A*A*A**
        
        if (ratio < 0.95) and (ratio > 0.0003) and (currArea >= 81): #up here to filter out small images; this can really slow down the process
            #use fromBook as an example of too many contours
            mask = np.zeros(thresh.shape,np.uint8)
            cv2.drawContours(mask,[cnt], 0, 255 ,-1)
            imgrayMean = cv2.mean(imgray, mask = mask)
            if (imgrayMean[0] < 0.75*tempAvg): 
            #cv2.rectangle(imgray, (x,y), (x+w, y+h), (0, 255, 0), 2)
                temp = imgray[y:y+h, x:x+w]
                #temp = cv2.resize(temp, (32, 32))
                
                plt.imshow(temp, cmap='gray')
                plt.title(str(h) + ", "  + str(w))
                plt.show()
                counter += 1
                rez.append(temp)
        
    print(counter)    
    return rez

def getIms(img):
    
    img, ret, thresh, avg, tempAvg = preProcessing(img)
    rez = analyzeContours(img, thresh, avg, tempAvg)
    
    return rez

img = cv2.imread('../evaluation/digit_training/2.png')
rez = getIms(img)
