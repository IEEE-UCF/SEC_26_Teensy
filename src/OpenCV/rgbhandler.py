import cv2
import numpy as np
import threading
import queue
import time
from collections import deque

# turns on OpenCV optimizations for better performance
cv2.setUseOptimized(True)
print(f"[System] OPENCV OPTIMIZATION ON!!!! {cv2.useOptimized()}")

# sets up threading and frame queue
stop_thread = threading.Event()
frame_queue = queue.Queue(maxsize=5)

def capture_frames():
    # runs in a separate thread, grabs frames, and dumps them into the queue
    cap = cv2.VideoCapture(0)
    cap.set(cv2.CAP_PROP_FRAME_WIDTH, 320)  # sets width to 320px
    cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 240)  # sets height to 240px
    cap.set(cv2.CAP_PROP_FPS, 30)  # sets FPS to 30
    
    try:
        while not stop_thread.is_set():
            ret, frame = cap.read()
            if ret and not frame_queue.full():
                frame_queue.put(cv2.UMat(frame))  # sends frame to GPU
    finally:
        cap.release()  # releases the camera when done
        print("[System] CAMERA SOURCES RELEASED!!!")

# starts the capture thread
threading.Thread(target=capture_frames, daemon=True).start()

def optimized_color_sample(umat_frame, x, y, size=5):
    # grabs a small area around (x, y) and gets the average color
    h, w = umat_frame.shape[:2]
    x1, y1 = max(0, x - size), max(0, y - size)
    x2, y2 = min(w, x + size), min(h, y + size)
    
    roi = umat_frame[y1:y2, x1:x2]  # grabs the region of interest
    return tuple(int(v) for v in cv2.mean(roi)[:3][::-1]) if roi.size else (0, 0, 0)  # flips BGR to RGB

# stores past color readings to smooth flickering
color_history = deque(maxlen=5)
fps_history = deque(maxlen=10)
SMOOTHING_FACTOR = 0.3
last_time = time.time()

while True:
    start_time = time.time()
    
    # if too many frames are waiting, drop old ones to keep it real-time
    with frame_queue.mutex:
        if frame_queue.qsize() > 1:
            frame_queue.queue.clear()
    
    if frame_queue.empty():
        continue  # nothing to process, skip this loop
    
    frame_umat = frame_queue.get()  # grabs the latest frame
    display_frame = cv2.UMat.get(frame_umat)  # converts UMat back to a normal image
    h, w = display_frame.shape[:2]
    
    # finds the center of the screen
    center_x, center_y = w // 2, h // 2
    raw_rgb = optimized_color_sample(frame_umat, center_x, center_y)  # grabs color at center
    
    # smooths color readings to stop flickering
    if color_history:
        smoothed_rgb = tuple(int(SMOOTHING_FACTOR * new + (1 - SMOOTHING_FACTOR) * old)
                         for new, old in zip(raw_rgb, color_history[-1]))
    else:
        smoothed_rgb = raw_rgb
    color_history.append(smoothed_rgb)
    
    # creates an overlay to show the detected color
    overlay = display_frame.copy()
    cv2.rectangle(overlay, (10, 50), (100, 100), smoothed_rgb, -1)  # draws a filled rectangle
    
    # blends the overlay with the main image for a smooth look
    np_overlay = cv2.UMat.get(overlay)
    np_display = cv2.UMat.get(display_frame)
    cv2.addWeighted(np_overlay, 0.7, np_display, 0.3, 0, np_display)
    
    # calculates FPS and smooths it
    fps = 1.0 / (time.time() - last_time)
    last_time = time.time()
    fps_history.append(fps)
    smoothed_fps = sum(fps_history) / len(fps_history) if fps_history else 0
    
    # draws a circle at the center to show where it's sampling color from
    cv2.circle(np_display, (center_x, center_y), 15, (255,255,255), 1)
    
    # displays the RGB values and FPS on screen
    cv2.putText(np_display, f"RGB: {smoothed_rgb}", (10, 30), 
                cv2.FONT_HERSHEY_SIMPLEX, 0.6, (255,255,255), 2)
    cv2.putText(np_display, f"FPS: {int(smoothed_fps)}", (10, 60),
                cv2.FONT_HERSHEY_SIMPLEX, 0.6, (0, 255, 0), 2)
    
    # shows the final processed frame
    cv2.imshow('THE SUPER RGBHANDLER!!!', np_display)
    
    # exits when 'q' is pressed
    if cv2.waitKey(1) & 0xFF == ord('q'):
        stop_thread.set()  # tells the capture thread to stop
        break

cv2.destroyAllWindows()
