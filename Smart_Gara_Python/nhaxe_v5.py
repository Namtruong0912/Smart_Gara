import cv2
import easyocr
import serial
import threading
import time
import numpy as np
import struct

# mở đường dẫn đến video
cap = cv2.VideoCapture(1, cv2.CAP_DSHOW)

# Tạo một đối tượng EasyOCR
reader = easyocr.Reader(['en'])

# Biến để kiểm soát luồng
running = True

# Biến để lưu giữ tín hiệu từ cổng COM
com_signal = None

# Hàm đọc dữ liệu từ cổng COM
def read_com_data():
    global com_signal
    while running:
        try:
            com_signal = ser.read().decode('utf-8')
            # com_signal = ser.readline().decode('utf-8').strip()
        except serial.SerialException:
            pass

# Hàm gửi dữ liệu ra qua cổng com

def send_result_to_com(ser, results):
    # Convert the results list to a formatted string
    # result_text = '\n'.join([f"Text: {text}, Probability: {prob}" for (_, text, prob) in results])
    result_text = ''.join([(text) for (_, text, _) in results])
    print(result_text.encode('ascii'))
    # Gửi dữ liệu kết quả
    ser.write(result_text.encode('ascii'))


# Mở kết nối đến cổng COM (điều chỉnh tên cổng COM tương ứng của bạn)
ser = serial.Serial('COM13', 9600, timeout=1)

# Bắt đầu luồng đọc dữ liệu từ cổng COM
com_thread = threading.Thread(target=read_com_data)
com_thread.start()

# Trạng thái hiển thị: 'video' hoặc 'captured_image'
display_state = 'video'

while True:
    # Đọc frame từ video stream
    ret, frame = cap.read()

    if not ret:
        print("Error reading frame")
        break

    # # Resize frame để có kích thước mong muốn
    # frame_resized = cv2.resize(frame, (frame_width, frame_height))

    # hiển thị video
    if display_state == 'video':
        cv2.imshow('Nha xe', frame)

    # Chụp ảnh và hiển thị sau khi nhận được kí tự "2"
    if com_signal == "1":
        cv2.imwrite('captured_image.jpg', frame)
        print("Captured image!")

        # Đọc biển số từ ảnh
        captured_image = cv2.imread('captured_image.jpg')

        # # Lấy kích thước của frame_resized
        # height_resized, width_resized, _ = frame_resized.shape

        # # Resize captured_image để có kích thước tương tự
        # captured_image_resized = cv2.resize(captured_image, (width_resized, height_resized))

        # Đọc biển số từ ảnh
        gray = cv2.cvtColor(captured_image, cv2.COLOR_BGR2GRAY)
        results = reader.readtext(gray)

        # Gửi giá trị biển số qua cổng com
        send_result_to_com(ser, results)

        # Hiển thị kết quả
        for (bbox, text, prob) in results:
            (top_left, top_right, bottom_right, bottom_left) = bbox
            top_left = tuple(map(int, top_left))
            bottom_right = tuple(map(int, bottom_right))
            cv2.rectangle(captured_image, top_left, bottom_right, (0, 255, 0), 2)
            cv2.putText(captured_image, text, top_left, cv2.FONT_HERSHEY_SIMPLEX, 0.8, (0, 255, 0), 2)

        # Hiển thị ảnh sau khi nhận diện
        cv2.imshow('Captured Image and Result', captured_image)

        # Đặt trạng thái hiển thị là 'captured_image'
        display_state = 'captured_image'
        cv2.destroyWindow('Nha xe')
        com_signal = None  # Đặt lại giá trị để tránh việc xử lý lặp lại

    # Kiểm tra phím nhấn
    key = cv2.waitKey(1)
    if display_state == 'captured_image':
        time.sleep(6)
        cv2.destroyWindow('Captured Image and Result')
        display_state = 'video'


#
# #*********************************************************************************************************************************
#
# # ---------------------------------------------------Kết hợp-------------------------------------------------------------
# # ..............................thư viện
# import cv2
# import easyocr
# import serial
# import threading
# import time
# import numpy as np
# import struct
#
# # ..............................Các biến
# # Biến để kiểm soát luồng
# running = True
#
# # Biến để lưu giữ tín hiệu từ cổng COM
# com_signal = None
#
# # Biến lưu trạng thái hiển thị video hoặc ảnh
# display_state = "video"
#
# # Biến màu va hình dạng
# color_result = None
# shape_result = None
#
# # ..............................Các hàm
# # Hàm đọc dữ liệu từ cổng COM
# def read_com_data():
#     global com_signal
#     while running:
#         try:
#             com_signal = ser.read().decode('utf-8')
#             # com_signal = ser.readline().decode('utf-8').strip()
#         except serial.SerialException:
#             pass
#
# # Hàm gửi dữ liệu ra qua cổng com
# def send_result_to_com(ser, results):
#     # Convert the results list to a formatted string
#     result_text = ''.join([text for (text,) in results])
#     # Gửi dữ liệu kết quả
#     ser.write(result_text.encode('utf-8'))
#
# # Hàm xử lý hình ảnh nhận diện màu và hình dạng
# def classify_product(image_path):
#     # Đọc biển số từ ảnh
#     gray = cv2.cvtColor(captured_image, cv2.COLOR_BGR2GRAY)
#     text_results = reader.readtext(gray)
#     print(text_results)
#
#     # Gửi giá trị biển số qua cổng com
#     send_result_to_com(ser, text_results)
#
#     # Hiển thị kết quả
#     for (bbox, text, prob) in text_results:
#         (top_left, top_right, bottom_right, bottom_left) = bbox
#         top_left = tuple(map(int, top_left))
#         bottom_right = tuple(map(int, bottom_right))
#         cv2.rectangle(captured_image, top_left, bottom_right, (0, 255, 0), 2)
#         cv2.putText(captured_image, text, top_left, cv2.FONT_HERSHEY_SIMPLEX, 0.8, (0, 255, 0), 2)
#
#     # Hiển thị ảnh sau khi nhận diện
#     cv2.imshow('Captured Image and Result', captured_image)
#
#     # Đặt trạng thái hiển thị là 'captured_image'
#     display_state = 'captured_image'
#     cv2.destroyWindow('video')
#
# # ................................Setup
#
# # Mở kết nối đến cổng COM (điều chỉnh tên cổng COM tương ứng của bạn)
# ser = serial.Serial('COM15', 9600, timeout=1)
#
# # Bắt đầu luồng đọc dữ liệu từ cổng COM
# com_thread = threading.Thread(target=read_com_data)
# com_thread.start()
#
# # Mở kết nối đến video stream
# # cap = cv2.VideoCapture(video_url)
# cap = cv2.VideoCapture(1, cv2.CAP_DSHOW)
#
# # Tạo một đối tượng EasyOCR
# reader = easyocr.Reader(['en'])
#
# # .....................................loop
# while True:
#     ret, frame = cap.read()
#
#     # Kiểm tra xem việc đọc frame có thành công không
#     if not ret:
#         print("Error reading frame")
#         break
#
#     # Show video từ camera
#     if(display_state == "video"):
#         cv2.imshow("video", frame)
#
#     # So sánh tín hiệu từ cổng COM
#     if cv2.waitKey(1) & 0xFF == ord('1'):
#         cv2.imwrite('captured_image.jpg', frame)
#         print("Captured image!")
#         captured_image = cv2.imread('captured_image.jpg')
#         com_signal = None
#         classify_product(captured_image)
#         cv2.waitKey(1)
#         time.sleep(5)
#         display_state = "video"
#         cv2.destroyWindow("Captured Image and Result")
#
#     # # Kiểm tra phím nhấn để tắt ảnh
#     # if cv2.waitKey(1) & 0xFF == ord('c'):
#     #     display_state = "video"
#     #     cv2.destroyWindow("anh da chup")
#
#     # Kiểm tra phím nhấn để thoát
#     if cv2.waitKey(1) & 0xFF == ord('o'):
#         break
#
# # Giải phóng tài nguyên và đóng cửa sổ
# cap.release()
# cv2.destroyAllWindows()


