'''
@author: Jess Smith

Last modified on 7 Jun 2014
Updates: Blurring for image processing, but this may be better if done before and on the entire image
    Got all 9 examples loaded (and in half the lines I thought it would take).
    Cheers for Jess!
Todo: Measure time taken with/without importing numpy and pyplot
    Measure that thing's speed
'''

import numpy as np
import cv2
import imageProcessing as iProc

# returns the pair train, classes
# classes is an array of string answers
def readImages():
    train = [] 
    classes = []        
    
    for i in range(0, 10):
        
        if (i != 200) and (i != 4):
            main = cv2.imread('../evaluation/digit_training/' + str(i) + '.png')
            arr = iProc.getIms(main)
    
            for img in arr:  
               median = cv2.medianBlur(img, 5)
               median = cv2.resize(median, (32,32))
               train.append(median)
               classes.append(i)

    main = cv2.imread('../evaluation/digit_training/+.png' )
    arr = iProc.getIms(main)
    for img in arr:
        median = cv2.medianBlur(img, 5)
        median = cv2.resize(median, (32, 32))
        train.append(median)
        classes.append(10) #10 is code for plus
      

    main = cv2.imread('../evaluation/digit_training/-.png' )
    arr = iProc.getIms(main)
    for img in arr:
        median = cv2.medianBlur(img, 5)
        median = cv2.resize(median, (32, 32))
        train.append(median)
        classes.append(11) #11 is code for minus
        
    train = np.asarray(train)
    classes = np.asarray(classes)
    
    return train, classes
    
train, classes = readImages()
print(train.shape)
print(classes.shape)