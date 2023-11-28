import serial
import binascii
import time

def serialConfig(globalSignalDelay = 92):
    ser = serial.Serial("/dev/ttyUSB0", 9600)
    if ser.isOpen():                        # 判断串口是否成功打开
        print("[Info]  : Serial Open.")
    else:
        print("[Error] : Serial Not Open.")
        return 1

    # b = hex(globalSignalDelay)[2:]
    b = '{:02x}'.format(globalSignalDelay)
    uarthex = bytes.fromhex('FF FF FF FF FF FF FF FF 64 05 90 00 ' + b +' F8 C8')
    write_len=ser.write(uarthex)

    time.sleep(0.2)
    count = ser.inWaiting()

    data = None
    if count > 0:
        data=ser.read(count)
        if data!=b'':
            dataStr = str(binascii.b2a_hex(data))[2:-1]
            # print("receive:",dataStr)
        else:
            return 2
        # if dataStr != 'ffffffffffffffff640590005cf8c8':
        #     return 3
    if data == None:
        return 4
    
    ser.close()
    if ser.isOpen():
        print("[Error] : Serial Not Close.")
    else:
        print("[Info]  : Serial Close. Uart send Done!")
    
    return 0

if __name__ == "__main__":
    serialConfig()