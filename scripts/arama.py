import socket
from enum import Enum

from typing import Literal

# from PIL import Image

from enum import IntEnum, Enum

class Input:
    class WiiUGamePad(IntEnum):
        A = 0x00008000
        B = 0x00004000
        X = 0x00002000
        Y = 0x00001000
        DP_Left = 0x00000800
        DPAD_Right = 0x00000400
        DPAD_Up = 0x00000200
        DPAD_Down = 0x00000100
        ZL = 0x00000080
        ZR = 0x00000040
        L = 0x00000020
        R = 0x00000010
        Plus = 0x00000008
        Minus = 0x00000004
        Home = 0x00000002
        Sync = 0x00000001
        TV = 0x00010000
        RS_Button = 0x00020000
        LS_Button = 0x00040000
        RS_Left = 0x04000000
        RS_Right = 0x02000000
        RS_Up = 0x01000000
        RS_Down = 0x00800000
        LS_Left = 0x40000000
        LS_Right = 0x20000000
        LS_Up = 0x10000000
        LS_Down = 0x08000000


class Command(bytes, Enum):
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

class IntType(int, Enum):
    Int8 = -1
    Int16 = -2
    Int32 = -4
    UInt8 = 1
    UInt16 = 2
    UInt32 = 4

class Version:
    major: int
    minor: int
    patch: int
    region: str

    def __str__(self) -> str:
        return f"{self.major}.{self.minor}.{self.patch}.{self.region}"
    
    def __repr__(self) -> str:
        return str(self)

class aRAMaClient:
    def __init__(self, ip: str, port: int = 7331) -> None:
        self.__socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.__socket.connect((ip, port))
        self.__socket.settimeout(5.0)

        self.__send(Command.SERVER_STATUS)
        success = bool.from_bytes(self.__recv(1), 'big')

        if not success:
            raise ConnectionError("status returned 0")
        
        self.__send(Command.GET_DATA_BUFFER_SIZE)
        self.buf_size = int.from_bytes(self.__recv(4), 'big')

        self.__send(Command.SERVER_VERSION)
        self.server_version = self.__recv().decode('ascii').rstrip('\x00')

        self.__send(Command.GET_OS_VERSION)
        self.os_version = Version()
        self.os_version.major = int.from_bytes(self.__recv(4), 'big')
        self.os_version.minor = int.from_bytes(self.__recv(4), 'big')
        self.os_version.patch = int.from_bytes(self.__recv(4), 'big')
        self.os_version.region = self.__recv(1).decode('ascii')
        self.__recv(3) # send because alignment

        self.__send(Command.GET_VERSION_HASH)
        self.version_hash = int.from_bytes(self.__recv(4), 'big')

    def __del__(self):
        self.__socket.close()

    def __recv(self, size: int = None):
        if size and not isinstance(size, int):
            raise ValueError("`size` must be int or None")

        if size:
            buffer = b""
            while len(buffer) < size:
                buffer += self.__socket.recv(size - len(buffer), 0)
            return buffer
        else:
            return self.__socket.recv(self.buf_size, 0)
        
    def __send(self, payload: bytes):
        if not isinstance(payload, bytes):
            raise ValueError("`payload` must be bytes")

        self.__socket.send(payload, 0)

    def pause(self):
        msg = Command.PAUSE_CONSOLE
        self.__send(msg)

    def resume(self):
        msg = Command.RESUME_CONSOLE
        self.__send(msg)

    def isPaused(self):
        msg = Command.IS_CONSOLE_PAUSED
        self.__send(msg)
        return bool.from_bytes(self.__recv(1), 'big')
    
    def read(self, start: int, end: int, kernel: bool = False):
        if kernel:
            msg = Command.READ_MEMORY_KERNEL
        else:
            msg = Command.READ_MEMORY
        msg += int.to_bytes(start, 4, 'big')
        msg += int.to_bytes(end, 4, 'big')
        if kernel:
            msg += int.to_bytes(1, 4, 'big')

        self.__send(msg)

        buffer = b""
        while len(buffer) < (end - start):
            only_zeroes = self.__recv(1) == b"\xB0"
            length = min(end - start, self.buf_size - 1)
            if only_zeroes:
                buffer += b"\0" * length
            else:
                buffer += self.__recv(length)

        return buffer
    
    def search(self, start: int, end: int, pattern: bytes | list[int], results: int, kernel: bool = False, aligned: bool = False):

        if not isinstance(pattern, bytes):
            pattern = bytes(pattern)

        msg = Command.ADVANCED_MEMORY_SEARCH
        msg += int.to_bytes(start, 4, 'big')
        msg += int.to_bytes(end - start, 4, 'big')
        msg += int.to_bytes(kernel, 4, 'big')
        msg += int.to_bytes(results, 4, 'big')
        msg += int.to_bytes(aligned, 4, 'big')
        msg += int.to_bytes(len(pattern), 4, 'big')
        msg += pattern

        self.__send(msg)
        length = int.from_bytes(self.__recv(4), 'big')
        buffer = self.__recv(length)
        addresses: list[int] = []
        for i in range(0, len(buffer), 4):
            addresses.append(int.from_bytes(buffer[i:i+4], 'big'))
        return addresses
    
    def write(self, start: int, value: int, dtype: IntType, kernel: bool = False):
        if kernel:
            if not dtype in [IntType.Int32, IntType.UInt32]:
                raise ValueError("using kernel requires 32bit values")

            msg = Command.KERNEL_WRITE
        else:
            if dtype in [IntType.Int8, IntType.UInt8]:
                msg = Command.WRITE_8
            elif dtype in [IntType.Int16, IntType.UInt16]:
                msg = Command.WRITE_16
            elif dtype in [IntType.Int32, IntType.UInt32]:
                msg = Command.WRITE_32
        
        signed = dtype < 0
        # length = abs(dtype)

        msg += int.to_bytes(start, 4, 'big')
        msg += int.to_bytes(value, 4, 'big', signed=signed)

        print(msg)

        self.__send(msg)

    def upload(self, start: int, value: bytes | list[int]):
        if isinstance(value, list):
            value = bytes(value)
        
        msg = Command.UPLOAD_MEMORY
        msg += int.to_bytes(start, 4, 'big')
        msg += int.to_bytes(start + len(value), 4, 'big')
        msg += value

        self.__send(msg)

    def fileRead(self, path: str):
        path = path.encode('ascii') + b"\0"

        msg = Command.READ_FILE
        msg += int.to_bytes(len(path), 4, 'big')
        msg += path

        self.__send(msg)

        if status := int.from_bytes(self.__recv(4), 'big', signed=True):
            raise ValueError(f"{hex(status)=}")
        
        size = int.from_bytes(self.__recv(4), 'big')
        buffer = b""
        while len(buffer) < size:
            buffer += self.__recv(size - len(buffer))
        return buffer

    def fileWrite(self, path: str, content: bytes):
        path = path.encode('ascii') + b"\0"

        msg = Command.REPLACE_FILE
        msg += int.to_bytes(len(path), 4, 'big')
        msg += path
        msg += int.to_bytes(len(content), 4, 'big')

        self.__send(msg)

        if status := int.from_bytes(self.__recv(4), 'big', signed=True):
            raise ValueError(f"{hex(status)=}")
        
        self.__send(content)

    def dirRead(self, path: str):
        path = path.encode('ascii') + b"\0"

        msg = Command.READ_DIRECTORY
        msg += int.to_bytes(len(path), 4, 'big')
        msg += path

        self.__send(msg)

        if status := int.from_bytes(self.__recv(4), 'big', signed=True):
            raise ValueError(f"{hex(status)=}")

        entries: list[str] = []
        while length := int.from_bytes(self.__recv(4), 'big'):
            entries.append(self.__recv(length).decode('ascii'))
        return entries

    def accountIdentifier(self):
        msg = Command.ACCOUNT_IDENTIFIER
        self.__send(msg)
        return int.from_bytes(self.__recv(4), 'big')
    
    def getSymbol(self, module: str, symbol: str, type: Literal['func', 'data'] = "func"):
        module = module.encode("ascii") + b"\0"
        symbol = symbol.encode("ascii") + b"\0"

        sub = int.to_bytes(8, 4, 'big')
        sub += int.to_bytes(8 + len(module), 4, 'big')
        sub += module
        sub += symbol

        msg = Command.GET_SYMBOL
        msg += int.to_bytes(len(sub), 1, 'big')
        msg += sub
        msg += int.to_bytes(1 if type == "data" else 0, 1, 'big')

        self.__send(msg)
        return int.from_bytes(self.__recv(4), 'big')
    
    def executeProcedure(self, address: int, *args: int):
        if len(args) > 8:
            raise ValueError("maximal 8 args")
        
        args = list(args) + [0] * (8 - len(args))

        msg = Command.REMOTE_PROCEDURE_CALL
        msg += int.to_bytes(address, 4, 'big')
        for arg in args:
            msg += int.to_bytes(arg, 4, 'big')

        self.__send(msg)
        return self.__recv(8)

    """
    def screenshot(self):
        msg = Command.TAKE_SCREEN_SHOT
        self.__send(msg)

        imageSize = int.from_bytes(self.__recv(4), 'big')
        print(f"{imageSize}")

        image = self.__recv(imageSize)
        print(image[0:50])
        # image = Image.frombytes('RGBA', (4, 4), image)
        # image.save(f"{__file__.replace('/arama.py', '/frame.png')}")
    """

    def test(self, code: list[int]):
        msg = Command.GET_CODE_HANDLER_ADDRESS

        bytes = b""
        for c in code:
            bytes += int.to_bytes(c, 4, 'big')

        msg += int.to_bytes(len(bytes), 1, 'big')
        msg += bytes

        self.__send(msg)

# 0x1506b500 = Current Health (Wind Waker HD EUR)

if __name__ == "__main__":
    import time

    def list_as_hex(l: list):
        print(f"[{', '.join(f'0x{x:08x}' for x in l)}]")

    client = aRAMaClient("10.0.0.69")

    # s = client.search(0x1506b500, 0x1506c000, [0x2f], 10)
    # list_as_hex(s)

    client.write(0x1506b54f, 0x2f, IntType.UInt8)

    print(list_as_hex(client.read(0x1506b54f, 0x1506b54f+1)))

    exit()

    # client.screenshot()

    class Address(IntEnum):
        CONTAINER = 0x1506b501
        HEALTH = 0x1506b503
        GRAPPLING_HOOK = 0x1506b53f
        HOOKSHOT = 0x1506b54f

    class ItemID(IntEnum):
        NONE = 0xFF
        GRAPPLING_HOOK = 0x25
        HOOKSHOT = 0x2f

    def RAM_WRITE(address: int, value: int):
        return [0xAA000000, address, value, 0xDEADC0DE]

    def IF_INPUT(input: int):
        return [0x0E000000, input]

    def END_IF():
        return [0xD0000000, 0xDEADC0DE]

    def IF_EQUAL(address: int, value: int):
        return [0x03000000, address, value, 0]
    
    def IF_NOT_EQUAL(address: int, value: int):
        return [0x04000000, address, value, 0]
    
    def NOP():
        return [0, 0]

    code = [
        # *IF_INPUT(Input.WiiUGamePad.A | Input.WiiUGamePad.DPAD_Right),
        # *RAM_WRITE(Address.HEALTH, 0x0B)
        # *END_IF(),
        # *NOP(),
        # *IF_EQUAL(Address.HEALTH, 0x0C),
        # *RAM_WRITE(Address.HEALTH, 0x0B),
        # *END_IF(),
        # *NOP(),
        *RAM_WRITE(Address.HOOKSHOT, ItemID.HOOKSHOT)
    ] 

    client.test(code)


    # a = client.search(0x1500_0000, 0x1600_0000, [0, 12, 0, 10], 10)
    # list_as_hex(a)

    # b = int.from_bytes(client.read(0x1506b503, 0x1506b504), 'big')
    # print(b)

    # client.write(0x1506_b503, 1, IntType.UInt8)

    # print(client.dirRead("fs:/vol/external01/wiiu/"))
    # print(client.fileRead("fs:/vol/external01/wiiu/test.txt"))
    # client.fileWrite("fs:/vol/external01/wiiu/test.txt", b"!test!")
    # print(client.fileRead("fs:/vol/external01/wiiu/test.txt"))

    # print(hex(client.accountIdentifier()))

    # s = client.getSymbol("coreinit.rpl", "OSGetTitleID")
    # print(f"0x{s:08x}")

    # id = int.from_bytes(client.executeProcedure(s), 'big')
    # print(f"0x{id:016x}")
