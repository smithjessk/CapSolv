# -*- coding: utf-8 -*-
"""
Created on Tue Jun 10 11:40:54 2014

@author: Jess
"""

import cv2
import matplotlib.pyplot as plt
import numpy as np

img = cv2.imread('../evaluation/examples/other_def_int.png')
img = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
img = cv2.medianBlur(img, 11)
ret, img = cv2.threshold(img, 120, 255, 0)
print(img)
plt.imshow(img, cmap='gray'), plt.show()
print(img.shape)

arr = []
for i in range(0, img.shape[0]):
    for j in range(0, img.shape[1]):
        if img[i][j] == 0:
           arr.append( (i,j) )

print(len(arr))
np.set_printoptions()

#np.savetxt('data.txt', arr, '%5.2f')
np.savetxt('data_unformatted.txt', arr)