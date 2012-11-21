
******************************** Commands Start ********************************
STI #10, 1(DS)
LOD A, 0(DS)
LOD B, 1(DS)
SUB A, B
BNG A, Rot0
JMP Rot1

Rot0:
LDI A, #1

Rot1:
STO A, 2(DS)

Rot2:
LOD A, 2(DS)
BZR A, Rot3
STI #1, 1(DS)
LOD A, 0(DS) ATW_COMMENT: T + T
LOD B, 1(DS)
ADD A, B
STO A, 2(DS) ATW_COMMENT: FIM
LOD A, 2(DS) ATW_COMMENT: ID = EXP
STO A, 0(DS)
STIF #0.05, 1(DS)
LOD A, 1(DS) ATW_COMMENT: PAUSA EXP
TME A ATW_COMMENT: FIM PAUSA
JMP Rot2

Rot3:

******************************** Commands Finish *******************************
HLT
