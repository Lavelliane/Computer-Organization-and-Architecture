WB 	0x09		; MBR = 0x09
WACC			; ACC = 0x09
SWAP			; IOBR = 0x09,MBR = 0x00
WB	0x00		; MBR = 0x00
WIO 	0x001		; IOData[0x001] = 0x09
BRE	0x016		; ACC = 0x09 - 0x00
WB	0x01		; MBR = 0x01
SUB			; ACC = 0x09 - 0x01 = 0x08
RACC			; MBR = 0x08
BR	0x004		; loop
EOP