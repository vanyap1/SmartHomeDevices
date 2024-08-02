import time
import threading
import socket
from enum import Enum
from support import ExternalHelper



HOST_BROADCAST = '192.168.1.255'
TX_PORT = 3001


class MsgType(Enum):
    RTC_SYNC = 0x81
    MSG	= 0x82
    POWERBANK = 0x83
    ALARM = 0x84




class MainThread():
    def __init__(self):
        pass

    def run(self): 
        while True:
            #dataArray = ExternalHelper.build_byte_array()
            
            dataArray = ExternalHelper.buildMessage("Python3") 
            send_byte_array(dataArray, HOST_BROADCAST, TX_PORT)
            print(dataArray)
            time.sleep(30)



def send_byte_array(byte_array, host, port):
    data = bytes(byte_array)
    udp_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    udp_socket.sendto(data, (host, port))



def udp_listener():
    
    udp_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    udp_socket.bind(('', 3000))

    while True:
        data, addr = udp_socket.recvfrom(1024)
        # {9:254:254:129:130:255:1:0}:{0:56:0:5:13:0:61:0:50}
        # {9:254:254:129:144:255:1:0}:{0:20:1:5:13:0:61:0:50}
        #  uint8_t rxtxBuffLenght;
	    #  uint8_t destinationAddr;
	    #  uint8_t senderAddr;
	    #  uint8_t opcode;
	    #  int16_t rssi;
	    #  uint8_t dataValid;
	    #  uint8_t dataCRC;                                             
        #                                        
        #  uint8_t second;        //!< 0-59
	    #  uint8_t minute;        //!< 0-59
	    #  uint8_t hour;          //!< 0-23
	    #  uint8_t date;          //!< 0-30 \note First day of month is 0, not 1.
	    #  uint8_t month;         //!< 0 January - 11 December
	    #  uint16_t year;         //!< 1970-2105
	    #  uint8_t dayofweek;     //!< 0 Sunday  - 6 Saturday 
        # 
         
                                           
        if (data[3] == MsgType.MSG.value):
            try:

                decoded_data = data[8:].decode('utf-8')
                print(decoded_data)
            except UnicodeDecodeError:
                hex_data = ":".join("{:02x}".format(byte) for byte in data)
                print(hex_data)

        if (data[3] == MsgType.RTC_SYNC.value):
            hex_data = ":".join("{:02x}".format(byte) for byte in data)
            dec_data = ":".join((str(byte))for byte in data)
            print(hex_data, dec_data)


            

if __name__ == '__main__':
    main = MainThread()
    main_thread = threading.Thread(target=main.run)
    udpListener = threading.Thread(target=udp_listener)

    main_thread.start()
    udpListener.start()

    main_thread.join()
    udpListener.join()
