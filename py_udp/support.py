import datetime

class ExternalHelper:
    @staticmethod
    def build_byte_array():
        now = datetime.datetime.now()

        byte_array = [
            0xaa,
            0x81,
            now.second,
            now.minute,
            now.hour,
            now.day,
            now.month,
            0,
            now.year-1970,
            0,
            now.weekday()
        ]
        return byte_array

    @staticmethod
    def buildMessage(msg):
        data_array = bytearray()
        data_array.append(0xaa)
        data_array.append(0x82)
        data_array.extend(msg.encode('utf-8'))
        return data_array