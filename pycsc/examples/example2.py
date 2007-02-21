#! /usr/bin/env python

import PyCSC

p = PyCSC.PyCSCLib()
print p.listReaders()

card = p.card()
print "Connected to reader: " + card.readerName

print "ATR: " + card.atr

apdu = "00A40000023F00"
print "Transmit: " + apdu
(resp, sw) = card.transmit(apdu)
print "resp: " + resp
print "sw: %s (%s)" % (sw, card.iso7816(sw))

