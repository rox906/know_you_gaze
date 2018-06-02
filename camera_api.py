import numpy as np
import cv2
import time

# 0号摄像头，也可以1、2，lsusb查看
cap = cv2.VideoCapture(0)
# cap2 = cv2.VideoCapture(1)

# 设置分辨率W
cap.set(3, 352)
cap.set(4, 288)
time.sleep(2)   # 必须要此步骤，否则失败
cap.set(15, 0)  # exposure
# cap2.set(3, 1280)
# cap2.set(4, 1024)
time.sleep(2)
# cap2.set(15, 0)

# 只能是如下选择分辨率.
# 160.0 x 120.0
# 176.0 x 144.0
# 320.0 x 240.0
# 352.0 x 288.0
# 640.0 x 480.0
# 1024.0 x 768.0
# 1280.0 x 1024.0

# 获取参数
fps = cap.get(5)
size = (int(cap.get(3)),
        int(cap.get(4)))

# fourcc = cv2.VideoWriter_fourcc(*'XVID')
# out = cv2.VideoWriter('out.avi', fourcc, fps, size)
# out2 = cv2.VideoWriter('out2.avi', fourcc, fps, size)

while cap.isOpened(): #and cap2.isOpened():
    ret, frame = cap.read()
    # ret2, frame2 = cap2.read()
    if ret:# and ret2:
        frame = cv2.flip(frame, 1)
        # frame2 = cv2.flip(frame2, 1)
        gray = cv2.cvtColor(frame,  cv2.COLOR_BGR2GRAY)
        # out.write(frame)
        # out2.write(frame2)

        # eye_cascade = cv2.CascadeClassifier('./py3/lib/python3.6/'+
        # 'site-packages/cv2/data/haarcascade_eye.xml')
        # eye_cascade_glasses = cv2.CascadeClassifier('./py3/lib/pyt'+
        # 'hon3.6/site-packages/cv2/data/haarcascade_eye_tree_eyeglasses.xml')
        # eyes = eye_cascade.detectMultiScale(gray)
        # glasses = eye_cascade_glasses.detectMultiScale(gray)
        # for (ex, ey, ew, eh) in eyes:
        #     cv2.rectangle(frame, (ex, ey), (ex+ew, ey+eh), (0, 255, 0), 2)
        #     break
        # for (ex, ey, ew, eh) in glasses:
        #     cv2.rectangle(frame, (ex, ey), (ex+ew, ey+eh), (0, 255, 0), 2)

        cv2.imshow('frame', frame)
        # cv2.imshow('frame2', frame2)

        if cv2.waitKey(1) & 0xFF == ord('q'):
            break
    else:
        break

cap.release()
# out.release()
cv2.destroyAllWindows()