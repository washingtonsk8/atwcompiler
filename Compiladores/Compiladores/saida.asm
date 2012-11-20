
******************************** Commands Start ********************************
STI #4, 1(DS)
STI #5, 2(DS)
LOD A, 1(DS)
LOD B, 2(DS)
SUB A, B
BPS A, Rot0
JMP Rot1

Rot0:
LDI A, #1

Rot1:
STO A, 3(DS)

Rot2:
LOD A, 3(DS)
BZR A, Rot3
JMP Rot2

Rot3:

******************************** Commands Finish *******************************
HLT
