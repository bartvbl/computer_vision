import numpy as np
import cv2

cap = cv2.VideoCapture(0)

while(True):
	# Capture frame-by-frame
	ret, image = cap.read()

	#kernel_small = np.ones((5, 5), np.uint8)
	kernel_large = np.ones((10, 10), np.uint8)

	dilated = cv2.dilate(image, kernel_large, iterations = 1)
	eroded = cv2.erode(dilated, kernel_large, iterations = 1)

	cv2.absdiff(image, eroded, image)

	#image = cv2.GaussianBlur(image, (5, 5), 0)

	#blurred = cv2.GaussianBlur(image, (0, 0), 3);
	#cv2.addWeighted(image, 1.5, blurred, -0.5, 0, image);

	image = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)

	ret3, image = cv2.threshold(image, 0, 255, cv2.THRESH_BINARY+cv2.THRESH_OTSU)

	#image = cv2.Canny(image, 100, 200)

	# Display the resulting frame
	cv2.imshow('frame', image)
	if cv2.waitKey(1) & 0xFF == ord('q'):
		break

# When everything done, release the capture
cap.release()
cv2.destroyAllWindows()