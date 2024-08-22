import numpy as np
from enum import Enum, IntEnum
import socket

from typing import Literal

class IntType(str, Enum):
    UINT8 = ">u1"
    UINT16 = ">u2"
    UINT32 = ">u4"
    UINT64 = ">u8"
    INT8 = ">i1"
    INT16 = ">i2"
    INT32 = ">i4"
    INT64 = ">i8"

def tobytes(value: int, *args, dtype: IntType = IntType.UINT32):
    return np.array([value, *args], dtype=dtype).tobytes()

np.set_printoptions(formatter={'int':lambda x:hex(int(x))})

class Address:
    def __init__(self, addr: int):
        self.__addr = np.array([addr], dtype=">u4")
    
    def __bytes__(self):
        return self.__addr.tobytes()
    
    def list(self):
        return list(bytes(self))

    def __add__(self, other):
        if isinstance(other, int):
            return Address(self.__addr + other)
        elif isinstance(other, Address):
            return Address(self.__addr + other.__addr)
        else:
            raise NotImplementedError()
        
    def __iadd__(self, other):
        if isinstance(other, int):
            self.__addr += other
            return self
        elif isinstance(other, Address):
            self.__addr += other.__addr
            return self
        else:
            raise NotImplementedError()
        
    def __sub__(self, other):
        if isinstance(other, int):
            return Address(self.__addr - other)
        elif isinstance(other, Address):
            return Address(self.__addr - other.__addr)
        else:
            raise NotImplementedError()
    
    def __isub__(self, other):
        if isinstance(other, int):
            self.__addr -= other
            return self
        elif isinstance(other, Address):
            self.__addr -= other.__addr
            return self
        else:
            raise NotImplementedError()
        
    def __lt__(self, other):
        if isinstance(other, int):
            return self.__addr < other
        elif isinstance(other, Address):
            return self.__addr < other.__addr
        else:
            raise NotImplementedError()
        
    def __le__(self, other):
        if isinstance(other, int):
            return self.__addr <= other
        elif isinstance(other, Address):
            return self.__addr <= other.__addr
        else:
            raise NotImplementedError()
        
    def __gt__(self, other):
        if isinstance(other, int):
            return self.__addr > other
        elif isinstance(other, Address):
            return self.__addr > other.__addr
        else:
            raise NotImplementedError()
        
    def __ge__(self, other):
        if isinstance(other, int):
            return self.__addr >= other
        elif isinstance(other, Address):
            return self.__addr >= other.__addr
        else:
            raise NotImplementedError()
        
    def __repr__(self) -> str:
        return f"0x{self.__addr:08x}"


class Command:
    WRITE_8 = b"\x01"
    WRITE_16 = b"\x02"
    WRITE_32 = b"\x03"
    READ_MEMORY = b"\x04"
    READ_MEMORY_KERNEL = b"\x05"
    VALIDATE_ADDRESS_RANGE = b"\x06"
    MEMORY_DISASSEMBLE = b"\x08"
    READ_MEMORY_COMPRESSED = b"\x09"  # TODO Remove command when done and integrate in read memory
    KERNEL_WRITE = b"\x0B"
    KERNEL_READ = b"\x0C"
    TAKE_SCREEN_SHOT = b"\x0D"  # TODO Finish this
    UPLOAD_MEMORY = b"\x41"
    SERVER_STATUS = b"\x50"
    GET_DATA_BUFFER_SIZE = b"\x51"
    READ_FILE = b"\x52"
    READ_DIRECTORY = b"\x53"
    REPLACE_FILE = b"\x54"  # TODO Test this
    GET_CODE_HANDLER_ADDRESS = b"\x55"
    READ_THREADS = b"\x56"
    ACCOUNT_IDENTIFIER = b"\x57"
    # WRITE_SCREEN = b"\x58"  # TODO Exception DSI
    FOLLOW_POINTER = b"\x60"
    REMOTE_PROCEDURE_CALL = b"\x70"
    GET_SYMBOL = b"\x71"
    MEMORY_SEARCH_32 = b"\x72"
    ADVANCED_MEMORY_SEARCH = b"\x73"
    EXECUTE_ASSEMBLY = b"\x81"
    PAUSE_CONSOLE = b"\x82"
    RESUME_CONSOLE = b"\x83"
    IS_CONSOLE_PAUSED = b"\x84"
    SERVER_VERSION = b"\x99"
    GET_OS_VERSION = b"\x9A"
    SET_DATA_BREAKPOINT = b"\xA0"
    SET_INSTRUCTION_BREAKPOINT = b"\xA2"
    TOGGLE_BREAKPOINT = b"\xA5"
    REMOVE_ALL_BREAKPOINTS = b"\xA6"
    POKE_REGISTERS = b"\xA7"
    GET_STACK_TRACE = b"\xA8"
    GET_ENTRY_POINT_ADDRESS = b"\xB1"
    RUN_KERNEL_COPY_SERVICE = b"\xCD"
    IOSU_HAX_READ_FILE = b"\xD0"
    GET_VERSION_HASH = b"\xE0"
    PERSIST_ASSEMBLY = b"\xE1"
    CLEAR_ASSEMBLY = b"\xE2"

class Gecko:
    def __init__(self, ip: str, port: int):
        self.ip = ip
        self.port = port
        self.__socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.__socket.connect((self.ip, self.port))

        self.buf_size = 0xFFF
        if not int.from_bytes(self.serverOnline(), "big"):
            return
        self.buf_size = int.from_bytes(self.serverBufferSize(), "big")


    def __del__(self):
        self.__socket.close()

    def send(self, payload: bytes, flags: socket.MsgFlag = None):
        self.__socket.send(payload, flags if flags else 0)

    def read(self, flags: socket.MsgFlag = None):
        return self.__socket.recv(self.buf_size, flags if flags else 0)
    
    def readMemory(self, start: int, end: int, useKernel: bool):
        if useKernel:
            msg = Command.READ_MEMORY_KERNEL
        else:
            msg = Command.READ_MEMORY
        msg += tobytes(start)
        msg += tobytes(end)
        if useKernel:
            msg += tobytes(1)
        
        self.send(msg)
        result = self.read()

        # check if memory was all zeroes
        return result[0] == 0xBD, result[1:]

    def validateAddressRange(self, start: int, end: int):
        msg = Command.VALIDATE_ADDRESS_RANGE
        msg += tobytes(start)
        msg += tobytes(end)

        self.send(msg)
        return self.read()
    
    def writeMemory(self, address: int, value: int, dtype: IntType = IntType.UINT8):
        if dtype in (IntType.UINT32, IntType.INT32):
            msg = Command.WRITE_32
        elif dtype in (IntType.UINT16, IntType.INT16):
            msg = Command.WRITE_16
        else:
            msg = Command.WRITE_8
        msg += tobytes(address)
        msg += tobytes(value, dtype=dtype)

        self.send(msg)

    def searchMemory(self, start: int, length: int, value: int):

        # make this call searchMemoryEx but only once it actually works

        msg = Command.MEMORY_SEARCH_32
        msg += tobytes(start)
        msg += tobytes(value, dtype=IntType.INT32)
        msg += tobytes(length)

        self.send(msg)
        temp = b""
        while (len(temp) < length):
            temp += self.read()
        return temp
    
    def searchMemoryEx(self, start: int, length: int, maxLength: int, useKernel: bool, aligned: int, searchByteCount: int):
        msg = Command.ADVANCED_MEMORY_SEARCH
        msg += tobytes(start)
        msg += tobytes(maxLength)
        msg += tobytes(useKernel)
        msg += tobytes(length)
        msg += tobytes(aligned)
        msg += tobytes(searchByteCount)

        self.send(msg)
        temp = b""
        while (len(temp) < length):
            temp += self.read()
        return temp

    def serverOnline(self):
        msg = Command.SERVER_STATUS

        self.send(msg)
        return self.read()
    
    def serverBufferSize(self):
        msg = Command.GET_DATA_BUFFER_SIZE

        self.send(msg)
        return self.read()
    
    def serverVersionHash(self):
        msg = Command.GET_VERSION_HASH

        self.send(msg)
        return self.read()

    def getSymbol(self, module: str, symbol: str, type: Literal["func", "data"] = "func"):
        module = bytes(module, "ascii") + b"\x00"
        symbol = bytes(symbol, "ascii") + b"\x00"

        sub  = tobytes(8)
        sub += tobytes(8 + len(module))
        sub += module
        sub += symbol

        msg = Command.GET_SYMBOL
        msg += tobytes(len(sub), dtype=IntType.UINT8)
        msg += sub
        msg += tobytes(1 if type == "data" else 0, dtype=IntType.UINT8)

        self.send(msg)
        return self.read()

    def executeProcedure(self, address: int, *args: int):
        if len(args) > 8:
            raise ValueError("maximal 8 args can be set")
        args = list(args) + [0] * (8 - len(args))
        
        print(list(map(hex, args)))

        msg = Command.REMOTE_PROCEDURE_CALL
        msg += tobytes(address)
        msg += tobytes(*args)

        print(msg)

        self.send(msg)
        return self.read()
