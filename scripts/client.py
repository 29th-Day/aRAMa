# echo-client.py

from common import Gecko, IntType
import numpy as np

HOST = "10.0.0.69"  # The server's hostname or IP address
PORT = 7331  # The port used by the server

gecko = Gecko(HOST, PORT)

OSIsAddressValid = gecko.getSymbol("coreinit.rpl", "OSIsAddressValid")

OSIsAddressValid = int.from_bytes(OSIsAddressValid, 'big')


r = gecko.executeProcedure(OSIsAddressValid, 0x010f4000)

print(r)
