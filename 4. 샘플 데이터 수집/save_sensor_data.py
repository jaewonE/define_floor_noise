import serial

# Set up the serial connection
SERIAL_PORT = '/dev/cu.usbmodem11101'
SERIAL_PORT = '/dev/cu.usbmodem1201'
BAUD_RATE = 9600
OUTPUT_FILE = '/Users/jaewone/Downloads/code/sensor_data_quite.txt'

with serial.Serial(SERIAL_PORT, BAUD_RATE) as ser, open(OUTPUT_FILE, 'w') as file:
    print("Recording data. Press Ctrl+C to stop...")
    try:
        while True:
            line = ser.readline().decode('ascii').strip()  # utf-8
            if line:  # 데이터가 있는 경우에만 파일에 저장합니다.
                file.write(line + '\n')
    except KeyboardInterrupt:
        print("Data recording stopped. Data saved to", OUTPUT_FILE)
