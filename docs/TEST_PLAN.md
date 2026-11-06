# Bring-up and integration test plan

Run one test at a time. Keep the robot's wheels lifted during motor tests.

## Stage 1 — Main board

- [ ] Confirm the ESP32 board and selected Arduino board profile.
- [ ] Upload a simple Serial Monitor sketch.
- [ ] Confirm each IR sensor changes state over black tape and white floor.
- [ ] Record whether black reads LOW or HIGH; set `IR_BLACK_LEVEL` in firmware.
- [ ] Test each motor-driver input separately and record the actual wheel response.
- [ ] Establish which two motors form each side and whether either side needs reversed polarity.
- [ ] Confirm the OLED I2C address with an I2C scanner.
- [ ] Confirm DHT11 readings.
- [ ] Confirm PIR output changes with motion.
- [ ] Confirm ultrasonic distance readings and ECHO voltage level.
- [ ] Test RC522 version register and read a test card UID.

## Stage 2 — Camera and cloud

- [ ] Confirm ESP32-CAM board model and camera pinout.
- [ ] Upload camera firmware with correct camera model pin definitions.
- [ ] Confirm the camera joins Wi-Fi and prints its IP address.
- [ ] Open `http://CAMERA_IP/capture?uid=TEST001` from a browser on the same network.
- [ ] Confirm Apps Script receives the image and appends a row.
- [ ] Open the Apps Script `/exec` URL and verify dashboard loads.
- [ ] Verify image access is restricted to the intended viewers.

## Stage 3 — Robot integration

- [ ] Test line following at low speed with the robot restrained / wheels lifted first.
- [ ] Calibrate turn direction and speed for 90-degree corners.
- [ ] Test checkpoint detection separately from line-following.
- [ ] Verify the robot remains stopped at a checkpoint until a valid RFID read.
- [ ] Verify a card scan triggers one capture and one attendance row only.
- [ ] Verify robot resumes line following after successful RFID processing.
- [ ] Test obstacle stop and safe resume behavior.
- [ ] Run multiple synthetic attendance test cases.

## Test evidence for GitHub

Add your own photos / screenshots of the actual assembled robot, serial output, model wiring, successful upload, Drive image, spreadsheet row, and dashboard. Do not present mock output as real test evidence.
