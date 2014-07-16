"""
Last modified on 9 Jun 2014
Updates: Can now recognize the digits 0-9, except 4.
    2 might be problematic, but that's only because of bad training data.
    Training accuracy is usually either all correct or one incorrect
@todo: oh fuck i need to change this haha. can't copy their code
@author: Jess Smith

"""
import random
import numpy as np
import imageReading as imRead
import cv2
import matplotlib.pyplot as plt

#variables for later
SZ = 20
bin_n = 16
svm_params = dict( kernel_type = cv2.SVM_LINEAR,
                    svm_type = cv2.SVM_C_SVC,
                    C=2.67, gamma=5.383 )

#computes the Histogram of Ordered Gradients
#returns a 64 bit vector
def hog(img):
    gx = cv2.Sobel(img, cv2.CV_32F, 1, 0)
    gy = cv2.Sobel(img, cv2.CV_32F, 0, 1)
    mag, ang = cv2.cartToPolar(gx, gy)
    
    # quantizing binvalues in (0...16)
    bins = np.int32(bin_n * ang / (2 * np.pi))

    # divide into 4 sub-squares
    bin_cells = bins[:16, :16], bins[16:, :16], bins[:16, 16:], bins[16:, 16:]
    mag_cells = mag[:16,:16], mag[16:,:16], mag[:16,16:], mag[16:,16:]
    hists = [np.bincount(b.ravel(), m.ravel(), bin_n) for b, m in zip(bin_cells, mag_cells)]
    hist = np.hstack(hists)
#    print(hist)
    return hist

'''
train, responses = imRead.readImages() #read in raw images and values
responses = responses.astype(np.float32)
#print(responses)
#print(responses.dtype)

train_images = np.asarray([train[0]])
test_images = np.asarray([train[1]])
train_responses = np.asarray([responses[0]])
test_responses = np.asarray([responses[1]])

random_indices = random.sample(range(2, len(responses)), 150)
for num in range(2, len(responses)):
    if (num in random_indices):
        #print(test_images.shape)
        #print([np.asarray(train[num])].shape)
        test_images = np.concatenate([test_images, [np.asarray(train[num])]])
        test_responses = np.concatenate([test_responses, [responses[num]]])
    else:
        #print(train_images.shape)
        #print(np.asarray(train[num]).shape)
        train_images = np.concatenate([train_images, [np.asarray(train[num])]])
        train_responses = np.concatenate([train_responses, [responses[num]] ])


#print(train_images.shape)
#print(test_images.shape)
#print(train_responses.shape)
#print(test_responses.shape)
'''
'''train_images = train[:] #all but one five will be used for training
#train_images = np.concatenate([train_images, train[4:8]])
test_images = []

train_responses = responses[:]
#train_responses = np.concatenate([train_responses, responses[4:8]])
test_responses = []
#print(len(test_responses))
'''
# apply hog to each example in the training set
'''hogdata = [hog(example) for example in train_images] 
trainData = np.float32(hogdata).reshape(-1, 64)

############ TRAIN SVM ################
svm = cv2.SVM()
svm.train(trainData, train_responses, params=svm_params)
svm.save('svm_data.dat')

############ TEST SVM #################
hogdata = [hog(example) for example in test_images]
testData = np.float32(hogdata).reshape(-1, 64)
result = svm.predict_all(testData)

#print(result)
########### check accuracy? ################
counter = 0 
for i in range(0, len(test_responses)):
    if (test_responses[i] == result[i]):
        counter += 1
        #print(test_responses[i])
        #plt.imshow(test_images[i], cmap='gray')
        #plt.show()
        
print(counter)        

#mask = result==test_responses
#correct = np.count_nonzero(mask)
#print(correct * 100.0 / result.size)
'''

image = cv2.imread('../evaluation/examples/polynomial1.jpg')
hog(img)