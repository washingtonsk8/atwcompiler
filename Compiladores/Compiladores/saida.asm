
******************************** Commands Start ********************************
STI #1, 1(DS)
LOD A, 1(DS) ATW_COMMENT: ID = EXP
STO A, 0(DS)
STI #10, 1(DS)
LOD A, 0(DS)
LOD B, 1(DS)
SUB A, B
BNG A, ROT0
LDI A, #0
JMP ROT1

ROT0:
LDI A, #1

ROT1:
STO A, 2(DS)
LOD A, 2(DS) ATW_COMMENT: SE
BZR A, ROT2
STI #10, 1(DS)
LOD A, 1(DS) ATW_COMMENT: PAUSA EXP
CNV A, A
TME A ATW_COMMENT: FIM PAUSA
JMP ROT3

ROT2:

ROT3:

******************************** Commands Finish *******************************
HLT
