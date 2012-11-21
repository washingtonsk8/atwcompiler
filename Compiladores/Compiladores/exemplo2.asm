ROT1:
************************** Expressao ******
************************** Expressao ******
     LOD   A , 109(DS)
     LOD   B , 108(DS)
     SUB   A , B
     BNG   A , ROT2
     LDI   A , #0
     JMP   ROT3
ROT2:
     LDI   A , #1
ROT3:
     STO   A , 110(DS)
     LOD   A , 110(DS)
     BZR   A , ROT4
************************** Expressao ******
     STIF  #0.0100 , 110(DS)
     LODF  A , 110(DS)
     LOD   B , 109(DS)
     CNV   B , B
     MULF  A , B
     STOF  A , 112(DS)
************************** Expressao ******
     STIF  #0.1000 , 114(DS)
************************** Expressao ******
     STI   #0 , 116(DS)
************************** Expressao ******
     STI   #0 , 117(DS)
************************** Expressao ******
     STI   #0 , 118(DS)
************************** Expressao ******
     STI   #0 , 119(DS)
     LODF  A , 112(DS)
     LODF  B , 114(DS)
     LOD   C , 116(DS)
     CNV   C , C
     LOD   D , 117(DS)
     CNV   D , D
     LOD   E , 118(DS)
     CNV   E , E
     LOD   F , 119(DS)
     CNV   F , F
     LDI   A , #97
     RTR 
************************** Atribuicao ******
************************** Expressao ******
     STI   #1 , 110(DS)
     LOD   A , 109(DS)
     LOD   B , 110(DS)
     ADD   A , B
     STO   A , 111(DS)
     LOD   A , 111(DS)
     STO   A , 109(DS)
************************** Expressao ******
     STIF  #0.0500 , 110(DS)
     LODF  A , 110(DS)
     TME   A
     JMP   ROT1
ROT4:
     HLT 
