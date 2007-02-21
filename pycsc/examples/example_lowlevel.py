#!/usr/bin/python

import PyCSC.pycsc

# binascii is used to tranform hexstrings into binary strings
# and vice versa. the struct package could also be used.
import binascii

card = PyCSC.pycsc.pycsc()

#Print reader name
print "Connected to reader: " + card.status()["ReaderName"]

print "Card Protocol: " + str(card.status()["Protocol"])
print "Card State: " + str(card.status()["State"]) 


# Print ATR
print "Card Answer To Reset: " + binascii.b2a_hex(card.status()["ATR"])

# For T=0 transmit, use PyCSC.pycsc.SCARD_PROTOCOL_T0
# For T=1 transmit, use PyCSC.pycsc.SCARD_PROTOCOL_T1
binascii.b2a_hex(card.transmit("\x00\xA4\x00\x00\x02\x3F\x00", 
                 PyCSC.pycsc.SCARD_PROTOCOL_T0))

# Protocol argument is optional (default is PyCSC.pycsc.SCARD_PROTOCOL_T0)
resp = card.transmit(binascii.a2b_hex("00A40000023F00"))

print "APDU sent to card: Select MF"

print "Response: " + binascii.b2a_hex(resp)

print "APDU sent to card: Get Response"
print "Response: " + binascii.b2a_hex(card.transmit("\x00\xC0\x00\x00" + resp[-1]))

readerName = PyCSC.pycsc.listReader()[0]
# if reader is empty, should block until card is inserted
# if reader is not empty, should return right away
newState =  PyCSC.pycsc.getStatusChange(ReaderStates=[{'Reader': readerName, 'CurrentState':PyCSC.pycsc.SCARD_STATE_EMPTY}])

print newState

# Shoud return immediately with current state
newState =  PyCSC.pycsc.getStatusChange(ReaderStates=[{'Reader': readerName, 'CurrentState':PyCSC.pycsc.SCARD_STATE_UNAWARE}])

# Block until any change of state
newState =  PyCSC.pycsc.getStatusChange(ReaderStates=[{'Reader': readerName, 'CurrentState': newState[0]['EventState']}])

print newState


# This works:
PyCSC.pycsc.getStatusChange(ReaderStates=newState)

# And so does this (default value for CurrentState is SCARD_STATE_EMPTY):
# This code should block if the reader is empty
# This is NOT true on Windows. CurrentState/EventState seem to behave differently
# The only way to block is the one listed aove (i.e. read the state
# update CurrentState with the returned EventState and call getStatusChange again)
PyCSC.pycsc.getStatusChange(ReaderStates=[{'Reader': readerName}])

# Block until card insertion with a 10000 ms timeout
PyCSC.pycsc.getStatusChange(Timeout=10000, ReaderStates=[{'Reader': readerName}])


