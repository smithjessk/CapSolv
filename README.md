CapSol
======

Soon enough, this won't be so messy. :)

Basic pipeline: 

 - Load image
 
 - preProcessing:
	 - img = image (converted to grayscale if necessary)
	 - tempAvg = average intensity of img
	 - ret, thresh = threshold img at intensities 115, 255
	 - avg = average intensity of thresh
	 
 - analyzeContours:
	 - contours = contours on thresh
	 - imArea = thresh's area
	 - For each contour,
		 - Determine it's bounding rectangle and its area
		 - ratio = current area / imArea
		 - if (ratio < 0.95) and (ratio > 0.003) and (currArea >= 81)
			 - crop the image from imgray and add it to rez