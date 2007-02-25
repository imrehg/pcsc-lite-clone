""" Python class to communicate with smart cards using PC/SC API """

#   Copyright (C) 2006 Ludovic Rousseau (ludovic.rousseau@free.fr)
#
# This file is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA

import PyCSC.pycsc

# binascii is used to tranform hexstrings into binary strings
# and vice versa. the struct package could also be used.
import binascii

SCARD_PROTOCOL_T0 = PyCSC.pycsc.SCARD_PROTOCOL_T0
SCARD_PROTOCOL_T1 = PyCSC.pycsc.SCARD_PROTOCOL_T1
SCARD_PROTOCOL_ANY = SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1
SCARD_PROTOCOL_RAW = PyCSC.pycsc.SCARD_PROTOCOL_RAW

SCARD_SHARE_SHARED = PyCSC.pycsc.SCARD_SHARE_SHARED
SCARD_SHARE_EXCLUSIVE = PyCSC.pycsc.SCARD_SHARE_EXCLUSIVE
SCARD_SHARE_DIRECT = PyCSC.pycsc.SCARD_SHARE_DIRECT


class Card:
    def __init__(self, reader, mode, protocol):
        self.pycsc = PyCSC.pycsc.pycsc(reader = reader, mode = mode, protocol = protocol)
        self.readerName = self.pycsc.status()["ReaderName"]
        self.atr = binascii.b2a_hex(self.pycsc.status()["ATR"])

    def transmit(self, apdu):
        """
        Calls SCardTransmit

        @param apdu: APDU to send in hex notation
        @type apdu: string
        @return: (response, sw)
            - response: data sent by the card in hex notation
            - sw: status word in hex notation (for example "9000")
        @rtype: list of strings
        """
        resp = self.pycsc.transmit(binascii.a2b_hex(apdu))
        resp = binascii.b2a_hex(resp)
        # split result and SW
        return (resp[:-4], resp[-4:])

    def iso7816(self, sw):
        """
        Convert a status word in human readable text

        @param sw: status word (for example "9000")
        @type sw: string
        @return: human readable text (for example "Normal processing.")
        @rtype: string
        """
        if len(sw) != 4:
            raise Exception("SW (%s) is not 4 chars long" % sw)
        sw = sw.upper()
        sw1 = sw[:2]
        sw2 = sw[2:]
        if sw == "9000":
            return "Normal processing."
        if sw1 == "61":
            return "0x%s bytes of response still available." % sw2
        if sw1 == "62":
            r = "State of non-volatile memory unchanged. "
            if sw2 == "00":
                r += "No information given."
            if sw2 == "81":
                r += "Part of returned data my be corrupted."
            if sw2 == "82":
                r += "End of file/record reached before reading Le bytes."
            if sw2 == "83":
                r += "Selected file invalidated."
            if sw2 == "84":
                r += "FCI not formated according to 5.1.5."
            return r
        if sw1 == "63":
            r = "State of non-volatile memory changed. "
            if sw2 == "00":
                r += "No information given."
            if sw2 == "81":
                r += "File filled up by the last write."
            if sw2[0] == "C":
                r += "Counter: 0x%s." % sw2[1]
            return r
        if sw == "6400":
            return "State of non-volatile memory unchanged."
        if sw1 == "65":
            r = "State of non-volatile memory changed. "
            if sw2 == "81":
                r += "Memory failure."
            return r
        if sw1 == "66":
            return "Reserved for security-related issues."
        if sw == "6700":
            return "Wrong length."
        if sw1 == "68":
            r = "Functions in CLA not supported. "
            if sw2 == "81":
                r += "Logical channel not supported."
            if sw2 == "82":
                r += "Secure messaging not supported."
        if sw1 == "69":
            r = "Command not allowed. "
            if sw2 == "81":
                r += "Command incompatible with file structure."
            if sw2 == "82":
                r += "Security status not satisfied."
            if sw2 == "83":
                r += "Authentication method blocked."
            if sw2 == "84":
                r += "Referenced data invalidated."
            if sw2 == "85":
                r += "Conditions of use not satisfied."
            if sw2 == "86":
                r += "Command not allowed (no current EF)."
            if sw2 == "87":
                r += "Expected SM data objects missing."
            if sw2 == "88":
                r += "SM data objects incorrect."
        if sw1 == "6A":
            r = "Wrong parameter(s) P1-P2. "
            if sw2 == "80":
                r += "Incorrect parameters in the data field."
            if sw2 == "81":
                r += "Function not supported."
            if sw2 == "82":
                r += "File not found."
            if sw2 == "83":
                r += "Record not found."
            if sw2 == "84":
                r += "Not enough memory space in the file."
            if sw2 == "85":
                r += "Lc inconsistent with TLV structure."
            if sw2 == "86":
                r += "Incorrect parameters P1-P2."
            if sw2 == "87":
                r += "Lc inconsistent with P1-P2."
            if sw2 == "88":
                r += "Referenced data not found."
        if sw == "6B00":
            return "Wrong parameter(s) P1-P2."
        if sw1 == "6C":
            return "Wrong length Le: should be 0x%s" % sw2
        if sw == "6D00":
            return "Instruction code not supported or invalid."
        if sw == "6E00":
            return "Class not supported."
        if sw == "6F00":
            return "No precise diagnosis."
        return "Error not defined by ISO 7816"

class PyCSCLib:
    def listReaders(self):
        """
        Return the list of connected readers

        @return: list of connected readers
        @rtype: list of strings
        """
        return PyCSC.pycsc.listReader()

    def card(self, reader = "", mode = SCARD_SHARE_SHARED, protocol = SCARD_PROTOCOL_ANY):
        """
        Connect to a card in a reader

        @param reader: reader name ("" by default)
        @type reader: string
        @param mode: can be L{SCARD_SHARE_SHARED} (default),
        L{SCARD_SHARE_EXCLUSIVE} or L{SCARD_SHARE_DIRECT}
        @type mode: integer
        @param protocol: can be L{SCARD_PROTOCOL_T0},
        L{SCARD_PROTOCOL_T1}, L{SCARD_PROTOCOL_ANY} (default),
        L{SCARD_PROTOCOL_RAW}
        @type protocol: integer
        @return: a L{Card} object
        @rtype: L{Card}
        """
        c = Card(reader = reader, mode = mode, protocol = protocol)
        return c

if __name__ == "__main__":
    # sample test/debug code
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

    # test card.iso7816
    if 0:
        for sw1 in range (0, 0x100):
            for sw2 in range (0, 0x100):
                sw = "%02X%02X" % (sw1, sw2)
                error = card.iso7816(sw)
                if error != "Error not defined by ISO 7816":
                    print sw, error

