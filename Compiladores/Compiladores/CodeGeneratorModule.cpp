#include "CodeGeneratorModule.h"
#include "ATW_Helper.h"
//---------------------------------------------------------------------------------------------------------------------
using namespace ATW_HELPER;
//---------------------------------------------------------------------------------------------------------------------
CodeGeneratorModule::CodeGeneratorModule(void){}
//---------------------------------------------------------------------------------------------------------------------
CodeGeneratorModule::~CodeGeneratorModule(void){}
//---------------------------------------------------------------------------------------------------------------------
void CodeGeneratorModule::fixCode(Address _AddressAsm, Address _AddressBin, char* _value){
	_wBuffer[_AddressAsm] = _value;
	list<bitset<16>*>::iterator it = _memoryPositions.begin();

	for(int i = 0; it != _memoryPositions.end() && i < _AddressBin; it++, i++);

	bitset<16>* _pointer = (*it);
	(*_pointer) = ATWgetInt(_value);
}
//---------------------------------------------------------------------------------------------------------------------
void CodeGeneratorModule::initialize(const char* _icFile){
	this->setGroupID(COMPILER_GROUP);
	clearWritingBuffer();
	_lIIBF = 0;
	_hIIBF = 0;
	_currentI = 0;

	fManager = new FileManager();
	fManager->initialize(0, NULL);
	fManager->openFile(_icFile, GENERIC_WRITE, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL);
	_Memory = ATWMemory::getInstance();

	binaryFile.open("saida.exe", ios::out | ios::binary);
}
//---------------------------------------------------------------------------------------------------------------------
void CodeGeneratorModule::initialize(int _Argc, void** _Argv){
	if(_Argc < 1)
		_eManager->callHandlers(this->getGroupID(), INSUFFICIENT_ARGUMENTS, NULL);

	this->setGroupID(COMPILER_GROUP);
	clearWritingBufferFULL();
	_lIIBF = 0;
	_hIIBF = 0;
	_currentI = 0;
	_InstIndex = 0;
	_BinIndex = 0;

	fManager = new FileManager();
	fManager->initialize(0, NULL);
	fManager->openFile((char*)_Argv[0], GENERIC_WRITE, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL);
	_Memory = ATWMemory::getInstance();

	binaryFile.open("saida.exe", ios::out | ios::binary);
}
//---------------------------------------------------------------------------------------------------------------------
void CodeGeneratorModule::dispose(int _Argc, void** _Argv){
	fManager->dispose(0, NULL);
	delete fManager;
	fManager = 0x0;

	binaryFile.close();
}
//---------------------------------------------------------------------------------------------------------------------
void CodeGeneratorModule::clearWritingBufferFULL(){
	for(int i = 0; i < MAX_WRITING_BUFFER; i++)
		_wBuffer[i] = "";
}
//---------------------------------------------------------------------------------------------------------------------
void CodeGeneratorModule::clearWritingBuffer(){
	for(int i = 0; i < _hIIBF; i++)
		_wBuffer[i] = "";
}
//---------------------------------------------------------------------------------------------------------------------
void CodeGeneratorModule::insertCodeToWriteBin(int _Element, char* _String){
	if(_String != NULL)
	{
		if(((*_String) >= '0' && (*_String) <= '9') || (*_String) == '-')
		{//Se é para gravar imediato
			char* _Number = _String;
			string _ValueDecimal = "", _ValueInteger = "";
			//int    _ValueDecimal = 0, _ValueInteger = 0;
			bool _isFloat = false;

			//Copiar o valor do Number para dois temporarios se necessário
			while((*_Number) != '\0')
			{
				if((*_Number) == '.')
				{
					_isFloat = true;				
					if ((*_String) == '-'){//Se o número é negativo, a parte real também deverá ser
						_ValueDecimal += '-';
					}//end if
					_Number ++;
				}//end if

				//Concatenando a parte inteira e real
				if(!_isFloat)
					_ValueInteger += *_Number;
				else
					_ValueDecimal += *_Number;

				_Number++;
			}//end while

			if(_isFloat)
			{
				bool _IsNegative = false;
				string _ZeroCont = "0";

				if(_ValueDecimal.at(0) == '-')
					_IsNegative = true;

				if(!_IsNegative){
					int _Quantity = _ValueDecimal.size(), i;
					//Completando o número para conter 16 bits completos
					for(i = _Quantity; i < 3; i++)
						_ZeroCont.append("0");
					_ValueDecimal.append(_ZeroCont);
				}
				else{
					int _Quantity = _ValueDecimal.size(), i;
					//Completando o número para conter 16 bits completos
					for(i = _Quantity; i < 4; i++)
						_ZeroCont.append("0");
					_ValueDecimal.append(_ZeroCont);
				}//end else

				int _valueIntegerI = ATWgetInt(_ValueInteger);
				int _valueDecimalI = ATWgetInt(_ValueDecimal);

				bitset<16>* _bitsetInteger = new bitset<16>(_valueIntegerI);
				bitset<16>* _bitsetDecimal = new bitset<16>(_valueDecimalI);

				insertIntoBitSetBuffer(_bitsetInteger);
				insertIntoBitSetBuffer(_bitsetDecimal);
				_BinIndex+=2;
			}//end if	
			else
			{
				int _valueIntegerI = ATWgetInt(_ValueInteger);

				bitset<16>* _bitsetInteger = new bitset<16>(_valueIntegerI);

				insertIntoBitSetBuffer(_bitsetInteger);
				_BinIndex++;
			}//end else
		}//end if
		else
		{
			bitset<16>* _bitsetRegister = new bitset<16>();
			switch((*_String))
			{
			case 'A':
				*_bitsetRegister = BinaryGen::A;
				break;
			case 'B':
				*_bitsetRegister = BinaryGen::B;
				break;
			case 'C':
				*_bitsetRegister = BinaryGen::C;
				break;
			case 'D':
				*_bitsetRegister = BinaryGen::D;
				break;
			case 'E':
				*_bitsetRegister = BinaryGen::E;
				break;
			case 'F':
				*_bitsetRegister = BinaryGen::F;
				break;
			case 'R'://Rótulos
				*_bitsetRegister = (int)getLabelAddress(_String);
				break;
			case 'L':
				//Retrocorreção de LIXO
				break;
			default:
				printf("ERRO!!\n%s\n",_String);
				system("pause");
				exit(-1);
				break;
			}//end switch
			insertIntoBitSetBuffer(_bitsetRegister);
			_BinIndex++;
		}//end else
	}//end if
	else{
		bitset<16>* _bitsetInteger = new bitset<16>(_Element);
		insertIntoBitSetBuffer(_bitsetInteger);
		_BinIndex++;
	}//end else
} 
//---------------------------------------------------------------------------------------------------------------------
void CodeGeneratorModule::insertCodeToWriteAsm(char* _Code, int _codeIndex, bool _Overlap){
	if(_codeIndex > MAX_WRITING_BUFFER || _codeIndex < 0)
		_eManager->callHandlers(this->getGroupID(), FATAL_ERROR, NULL);

	if(_codeIndex == -1){
		_codeIndex = _currentI;

		if(_currentI < MAX_WRITING_BUFFER)
			_currentI ++;
		else
			return;
	}

	if(_wBuffer[_codeIndex] == ""){
		_wBuffer[_codeIndex] += _Code;
	}else if(_Overlap){
		_wBuffer[_codeIndex] = _Code;
	}else
		return;

	if(_codeIndex > _hIIBF)
		_hIIBF = _codeIndex;
	_lIIBF = _codeIndex;
}
//---------------------------------------------------------------------------------------------------------------------
string CodeGeneratorModule::getCode(int _codeIndex){
	if(_codeIndex > MAX_WRITING_BUFFER || _codeIndex < 0)
		_eManager->callHandlers(this->getGroupID(), FATAL_ERROR, NULL);

	if(_codeIndex == -1)
		_codeIndex = _lIIBF;

	return _wBuffer[_codeIndex];
}
//---------------------------------------------------------------------------------------------------------------------
void CodeGeneratorModule::flush(){
	for(int i = 0; i <= _hIIBF; i++){
		if(_wBuffer[i] != ""){
			fManager->writeInFile(_wBuffer[i].c_str());
			_wBuffer[i] = "";
		}
	}

	_currentI = 0;
	_lIIBF = 0;
	_hIIBF = 0;
}
//---------------------------------------------------------------------------------------------------------------------
void CodeGeneratorModule::flushBin(){
	list<bitset<16>*>::iterator it = _memoryPositions.begin();
	short forWriting = 0x00;

	for(; it != _memoryPositions.end(); it++){
		bitset<16> _pointer = (*(*it));
		cout << _pointer << " ";
		forWriting = static_cast<short>(_pointer.to_ulong());		
		writeS16(forWriting);
	}//end for
	cout << endl;
	clearBitSetBuffer();
	_BinIndex = 0;
}
//----------------------------------------------------------------------------------------------------------------------
void CodeGeneratorModule::writeS16(short val)
{
	char bytes[2];

	// extract the individual bytes from our value
	bytes[0] = (val) & 0xFF;  // low byte
	bytes[1] = (val >> 8) & 0xFF;  // high byte

	// write those bytes to the file
	binaryFile.write((char*)bytes, 2);
}
//----------------------------------------------------------------------------------------------------------------------
void CodeGeneratorModule::write(char* _String)
{
	insertCodeToWriteAsm(_String, _InstIndex++);
}
//----------------------------------------------------------------------------------------------------------------------
void CodeGeneratorModule::writeRot(char* _String)
{
	//ADDRESS
	Address _PCAddress = *_CurrentPCAdress;
	unsigned int _CurrentKey = SDBMHash(_String) % MAX_LABELS;	
	_Labels[_CurrentKey] = _PCAddress;
	//ADDRESS

	char _Rot[255] = "\n";
	strcat(_Rot, _String);
	strcat(_Rot, ":\n");
	insertCodeToWriteAsm(_Rot, _InstIndex++);
}
//----------------------------------------------------------------------------------------------------------------------
Address* CodeGeneratorModule::ADD(char* _RegD, char* _RegO, char* _Comment)
{
	Address* _InstIndexBase = (Address*) malloc (sizeof(Address)*2); memset(_InstIndexBase, 0, sizeof(Address)*2); _InstIndexBase[0] = _InstIndex; _InstIndexBase[1] = _BinIndex;
	insertCodeToWriteAsm("ADD ", _InstIndex++);
	insertCodeToWriteAsm(_RegD, _InstIndex++);	
	insertCodeToWriteAsm(", ", _InstIndex++);
	insertCodeToWriteAsm(_RegO, _InstIndex++);

	if(strcmp(_Comment, "") != 0){
		char _sPrint[255];
		sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);
		insertCodeToWriteAsm(_sPrint, _InstIndex++);
	}

	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 4 + 1

	//binary
	insertCodeToWriteBin(BinaryGen::ADD);
	insertCodeToWriteBin(NULL,_RegD);
	insertCodeToWriteBin(NULL,_RegO);

	_Memory->incrementPC(); return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
Address* CodeGeneratorModule::ADDF(char* _RegD, char* _RegO, char* _Comment)
{
	Address* _InstIndexBase = (Address*) malloc (sizeof(Address)*2); memset(_InstIndexBase, 0, sizeof(Address)*2); _InstIndexBase[0] = _InstIndex; _InstIndexBase[1] = _BinIndex;	
	insertCodeToWriteAsm("ADDF ", _InstIndex++);
	insertCodeToWriteAsm(_RegD, _InstIndex++);
	insertCodeToWriteAsm(", ", _InstIndex++);
	insertCodeToWriteAsm(_RegO, _InstIndex++);

	if(strcmp(_Comment, "") != 0){
		char _sPrint[255];
		sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);
		insertCodeToWriteAsm(_sPrint, _InstIndex++);
	}

	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 4 + 1
	insertCodeToWriteBin(BinaryGen::ADDF);
	insertCodeToWriteBin(NULL,_RegD);
	insertCodeToWriteBin(NULL,_RegO);

	_Memory->incrementPC(); return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
Address* CodeGeneratorModule::ADI(char* _RegD, char* _Imed, char* _Comment)
{
	Address* _InstIndexBase = (Address*) malloc (sizeof(Address)*2); memset(_InstIndexBase, 0, sizeof(Address)*2); _InstIndexBase[0] = _InstIndex; _InstIndexBase[1] = _BinIndex;
	insertCodeToWriteAsm("ADI ", _InstIndex++);
	insertCodeToWriteAsm(_RegD, _InstIndex++);
	insertCodeToWriteAsm(", #", _InstIndex++);
	insertCodeToWriteAsm(_Imed, _InstIndex++);

	if(strcmp(_Comment, "") != 0){
		char _sPrint[255];
		sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);
		insertCodeToWriteAsm(_sPrint, _InstIndex++);
	}

	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 4 + 1
	insertCodeToWriteBin(BinaryGen::ADI);
	insertCodeToWriteBin(NULL,_RegD);
	insertCodeToWriteBin(NULL,_Imed);

	_Memory->incrementPC(); return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
Address* CodeGeneratorModule::ADIF(char* _RegD, char* _Imed, char* _Comment)
{
	Address* _InstIndexBase = (Address*) malloc (sizeof(Address)*2); memset(_InstIndexBase, 0, sizeof(Address)*2); _InstIndexBase[0] = _InstIndex; _InstIndexBase[1] = _BinIndex;

	insertCodeToWriteAsm("ADIF ", _InstIndex++);
	insertCodeToWriteAsm(_RegD, _InstIndex++);
	insertCodeToWriteAsm(", #", _InstIndex++);
	insertCodeToWriteAsm(_Imed, _InstIndex++);

	if(strcmp(_Comment, "") != 0){
		char _sPrint[255];
		sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);
		insertCodeToWriteAsm(_sPrint, _InstIndex++);
	}

	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 4 + 1
	insertCodeToWriteBin(BinaryGen::ADIF);
	insertCodeToWriteBin(NULL,_RegD);
	insertCodeToWriteBin(NULL,_Imed);

	_Memory->incrementPC(); return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
Address* CodeGeneratorModule::BNG(char* _Reg, char* _Rot, char* _Comment){
	Address* _InstIndexBase = (Address*) malloc (sizeof(Address)*2); memset(_InstIndexBase, 0, sizeof(Address)*2); _InstIndexBase[0] = _InstIndex; _InstIndexBase[1] = _BinIndex;
	insertCodeToWriteAsm("BNG ", _InstIndex++);
	insertCodeToWriteAsm(_Reg, _InstIndex++);
	insertCodeToWriteAsm(", ", _InstIndex++);
	insertCodeToWriteAsm(_Rot, _InstIndex++);

	if(strcmp(_Comment, "") != 0){
		char _sPrint[255];
		sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);
		insertCodeToWriteAsm(_sPrint, _InstIndex++);
	}

	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 5 + 1	
	insertCodeToWriteBin(BinaryGen::BNG);
	insertCodeToWriteBin(NULL,_Reg);
	insertCodeToWriteBin(NULL,_Rot);

	_Memory->incrementPC(); return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
Address* CodeGeneratorModule::BNGF(char* _Reg, char* _Rot, char* _Comment)
{
	Address* _InstIndexBase = (Address*) malloc (sizeof(Address)*2); memset(_InstIndexBase, 0, sizeof(Address)*2); _InstIndexBase[0] = _InstIndex; _InstIndexBase[1] = _BinIndex;
	insertCodeToWriteAsm("BNGF ", _InstIndex++);
	insertCodeToWriteAsm(_Reg, _InstIndex++);
	insertCodeToWriteAsm(", ", _InstIndex++);
	insertCodeToWriteAsm(_Rot, _InstIndex++);

	if(strcmp(_Comment, "") != 0){
		char _sPrint[255];
		sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);
		insertCodeToWriteAsm(_sPrint, _InstIndex++);
	}

	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 5 + 1
	insertCodeToWriteBin(BinaryGen::BNGF);
	insertCodeToWriteBin(NULL,_Reg);
	insertCodeToWriteBin(NULL,_Rot);

	_Memory->incrementPC(); return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
Address* CodeGeneratorModule::BNN(char* _Reg, char* _Rot, char* _Comment)
{
	Address* _InstIndexBase = (Address*) malloc (sizeof(Address)*2); memset(_InstIndexBase, 0, sizeof(Address)*2); _InstIndexBase[0] = _InstIndex; _InstIndexBase[1] = _BinIndex;
	insertCodeToWriteAsm("BNN ", _InstIndex++);
	insertCodeToWriteAsm(_Reg, _InstIndex++);
	insertCodeToWriteAsm(", ", _InstIndex++);
	insertCodeToWriteAsm(_Rot, _InstIndex++);

	if(strcmp(_Comment, "") != 0){
		char _sPrint[255];
		sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);
		insertCodeToWriteAsm(_sPrint, _InstIndex++);
	}

	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 5 + 1
	insertCodeToWriteBin(BinaryGen::BNN);
	insertCodeToWriteBin(NULL,_Reg);
	insertCodeToWriteBin(NULL,_Rot);

	_Memory->incrementPC(); return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
Address* CodeGeneratorModule::BNNF(char* _Reg, char* _Rot, char* _Comment)
{
	Address* _InstIndexBase = (Address*) malloc (sizeof(Address)*2); memset(_InstIndexBase, 0, sizeof(Address)*2); _InstIndexBase[0] = _InstIndex; _InstIndexBase[1] = _BinIndex;
	insertCodeToWriteAsm("BNNF ", _InstIndex++);
	insertCodeToWriteAsm(_Reg, _InstIndex++);
	insertCodeToWriteAsm(", ", _InstIndex++);
	insertCodeToWriteAsm(_Rot, _InstIndex++);

	if(strcmp(_Comment, "") != 0){
		char _sPrint[255];
		sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);
		insertCodeToWriteAsm(_sPrint, _InstIndex++);
	}

	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 5 + 1
	insertCodeToWriteBin(BinaryGen::BNNF);
	insertCodeToWriteBin(NULL,_Reg);
	insertCodeToWriteBin(NULL,_Rot);

	_Memory->incrementPC(); return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
Address* CodeGeneratorModule::BNP(char* _Reg, char* _Rot, char* _Comment)
{
	Address* _InstIndexBase = (Address*) malloc (sizeof(Address)*2); memset(_InstIndexBase, 0, sizeof(Address)*2); _InstIndexBase[0] = _InstIndex; _InstIndexBase[1] = _BinIndex;
	insertCodeToWriteAsm("BNP ", _InstIndex++);
	insertCodeToWriteAsm(_Reg, _InstIndex++);
	insertCodeToWriteAsm(", ", _InstIndex++);
	insertCodeToWriteAsm(_Rot, _InstIndex++);

	if(strcmp(_Comment, "") != 0){
		char _sPrint[255];
		sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);
		insertCodeToWriteAsm(_sPrint, _InstIndex++);
	}

	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 5 + 1
	insertCodeToWriteBin(BinaryGen::BNP);
	insertCodeToWriteBin(NULL,_Reg);
	insertCodeToWriteBin(NULL,_Rot);

	_Memory->incrementPC(); return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
Address* CodeGeneratorModule::BNPF(char* _Reg, char* _Rot, char* _Comment)
{
	Address* _InstIndexBase = (Address*) malloc (sizeof(Address)*2); memset(_InstIndexBase, 0, sizeof(Address)*2); _InstIndexBase[0] = _InstIndex; _InstIndexBase[1] = _BinIndex;
	insertCodeToWriteAsm("BNPF ", _InstIndex++);
	insertCodeToWriteAsm(_Reg, _InstIndex++);
	insertCodeToWriteAsm(", ", _InstIndex++);
	insertCodeToWriteAsm(_Rot, _InstIndex++);

	if(strcmp(_Comment, "") != 0){
		char _sPrint[255];
		sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);
		insertCodeToWriteAsm(_sPrint, _InstIndex++);
	}

	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 5 + 1
	insertCodeToWriteBin(BinaryGen::BNPF);
	insertCodeToWriteBin(NULL,_Reg);
	insertCodeToWriteBin(NULL,_Rot);

	_Memory->incrementPC(); return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
Address* CodeGeneratorModule::BNZ(char* _Reg, char* _Rot, char* _Comment)
{
	Address* _InstIndexBase = (Address*) malloc (sizeof(Address)*2); memset(_InstIndexBase, 0, sizeof(Address)*2); _InstIndexBase[0] = _InstIndex; _InstIndexBase[1] = _BinIndex;
	insertCodeToWriteAsm("BNZ ", _InstIndex++);
	insertCodeToWriteAsm(_Reg, _InstIndex++);
	insertCodeToWriteAsm(", ", _InstIndex++);
	insertCodeToWriteAsm(_Rot, _InstIndex++);

	if(strcmp(_Comment, "") != 0){
		char _sPrint[255];
		sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);
		insertCodeToWriteAsm(_sPrint, _InstIndex++);
	}

	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 5 + 1
	insertCodeToWriteBin(BinaryGen::BNZ);
	insertCodeToWriteBin(NULL,_Reg);
	insertCodeToWriteBin(NULL,_Rot);

	_Memory->incrementPC(); return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
Address* CodeGeneratorModule::BNZF(char* _Reg, char* _Rot, char* _Comment)
{
	Address* _InstIndexBase = (Address*) malloc (sizeof(Address)*2); memset(_InstIndexBase, 0, sizeof(Address)*2); _InstIndexBase[0] = _InstIndex; _InstIndexBase[1] = _BinIndex;
	insertCodeToWriteAsm("BNZF ", _InstIndex++);
	insertCodeToWriteAsm(_Reg, _InstIndex++);
	insertCodeToWriteAsm(", ", _InstIndex++);
	insertCodeToWriteAsm(_Rot, _InstIndex++);

	if(strcmp(_Comment, "") != 0){
		char _sPrint[255];
		sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);
		insertCodeToWriteAsm(_sPrint, _InstIndex++);
	}

	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 5 + 1
	insertCodeToWriteBin(BinaryGen::BNZF);
	insertCodeToWriteBin(NULL,_Reg);
	insertCodeToWriteBin(NULL,_Rot);

	_Memory->incrementPC(); return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
Address* CodeGeneratorModule::BPS(char* _Reg, char* _Rot, char* _Comment)
{
	Address* _InstIndexBase = (Address*) malloc (sizeof(Address)*2); memset(_InstIndexBase, 0, sizeof(Address)*2); _InstIndexBase[0] = _InstIndex; _InstIndexBase[1] = _BinIndex;
	insertCodeToWriteAsm("BPS ", _InstIndex++);
	insertCodeToWriteAsm(_Reg, _InstIndex++);
	insertCodeToWriteAsm(", ", _InstIndex++);
	insertCodeToWriteAsm(_Rot, _InstIndex++);

	if(strcmp(_Comment, "") != 0){
		char _sPrint[255];
		sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);
		insertCodeToWriteAsm(_sPrint, _InstIndex++);
	}

	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 5 + 1
	insertCodeToWriteBin(BinaryGen::BPS);
	insertCodeToWriteBin(NULL,_Reg);
	insertCodeToWriteBin(NULL,_Rot);

	_Memory->incrementPC(); return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
Address* CodeGeneratorModule::BPSF(char* _Reg, char* _Rot, char* _Comment)
{
	Address* _InstIndexBase = (Address*) malloc (sizeof(Address)*2); memset(_InstIndexBase, 0, sizeof(Address)*2); _InstIndexBase[0] = _InstIndex; _InstIndexBase[1] = _BinIndex;
	insertCodeToWriteAsm("BPSF", _InstIndex++);
	insertCodeToWriteAsm(_Reg, _InstIndex++);
	insertCodeToWriteAsm(", ", _InstIndex++);
	insertCodeToWriteAsm(_Rot, _InstIndex++);

	if(strcmp(_Comment, "") != 0){
		char _sPrint[255];
		sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);
		insertCodeToWriteAsm(_sPrint, _InstIndex++);
	}

	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 5 + 1
	insertCodeToWriteBin(BinaryGen::BPSF);
	insertCodeToWriteBin(NULL,_Reg);
	insertCodeToWriteBin(NULL,_Rot);

	_Memory->incrementPC(); return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
Address* CodeGeneratorModule::BZR(char* _Reg, char* _Rot, char* _Comment)
{
	Address* _InstIndexBase = (Address*) malloc (sizeof(Address)*2); memset(_InstIndexBase, 0, sizeof(Address)*2); _InstIndexBase[0] = _InstIndex; _InstIndexBase[1] = _BinIndex;
	insertCodeToWriteAsm("BZR ", _InstIndex++);
	insertCodeToWriteAsm(_Reg, _InstIndex++);
	insertCodeToWriteAsm(", ", _InstIndex++);
	insertCodeToWriteAsm(_Rot, _InstIndex++);

	if(strcmp(_Comment, "") != 0){
		char _sPrint[255];
		sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);
		insertCodeToWriteAsm(_sPrint, _InstIndex++);
	}

	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 5 + 1
	insertCodeToWriteBin(BinaryGen::BZR);
	insertCodeToWriteBin(NULL,_Reg);
	insertCodeToWriteBin(NULL,_Rot);

	_Memory->incrementPC(); return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
Address* CodeGeneratorModule::BZRF(char* _Reg, char* _Rot, char* _Comment)
{
	Address* _InstIndexBase = (Address*) malloc (sizeof(Address)*2); memset(_InstIndexBase, 0, sizeof(Address)*2); _InstIndexBase[0] = _InstIndex; _InstIndexBase[1] = _BinIndex;
	insertCodeToWriteAsm("BZRF ", _InstIndex++);
	insertCodeToWriteAsm(_Reg, _InstIndex++);
	insertCodeToWriteAsm(", ", _InstIndex++);
	insertCodeToWriteAsm(_Rot, _InstIndex++);

	if(strcmp(_Comment, "") != 0){
		char _sPrint[255];
		sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);
		insertCodeToWriteAsm(_sPrint, _InstIndex++);
	}

	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 5 + 1
	insertCodeToWriteBin(BinaryGen::BZRF);
	insertCodeToWriteBin(NULL,_Reg);
	insertCodeToWriteBin(NULL,_Rot);

	_Memory->incrementPC(); return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
Address* CodeGeneratorModule::CNV(char* _RegD, char* _RegO, char* _Comment)
{
	Address* _InstIndexBase = (Address*) malloc (sizeof(Address)*2); memset(_InstIndexBase, 0, sizeof(Address)*2); _InstIndexBase[0] = _InstIndex; _InstIndexBase[1] = _BinIndex;
	insertCodeToWriteAsm("CNV ", _InstIndex++);
	insertCodeToWriteAsm(_RegD, _InstIndex++);
	insertCodeToWriteAsm(", ", _InstIndex++);
	insertCodeToWriteAsm(_RegO, _InstIndex++);

	if(strcmp(_Comment, "") != 0){
		char _sPrint[255];
		sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);
		insertCodeToWriteAsm(_sPrint, _InstIndex++);
	}

	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 4 + 1
	insertCodeToWriteBin(BinaryGen::CNV);
	insertCodeToWriteBin(NULL,_RegD);
	insertCodeToWriteBin(NULL,_RegO);

	_Memory->incrementPC(); return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
Address* CodeGeneratorModule::DIV(char* _RegD, char* _RegO, char* _Comment)
{
	Address* _InstIndexBase = (Address*) malloc (sizeof(Address)*2); memset(_InstIndexBase, 0, sizeof(Address)*2); _InstIndexBase[0] = _InstIndex; _InstIndexBase[1] = _BinIndex;
	insertCodeToWriteAsm("DIV ", _InstIndex++);
	insertCodeToWriteAsm(_RegD, _InstIndex++);
	insertCodeToWriteAsm(", ", _InstIndex++);
	insertCodeToWriteAsm(_RegO, _InstIndex++);

	if(strcmp(_Comment, "") != 0){
		char _sPrint[255];
		sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);
		insertCodeToWriteAsm(_sPrint, _InstIndex++);
	}

	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 4 + 1
	insertCodeToWriteBin(BinaryGen::DIV);
	insertCodeToWriteBin(NULL,_RegD);
	insertCodeToWriteBin(NULL,_RegO);

	_Memory->incrementPC(); return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
Address* CodeGeneratorModule::ESC(char* _Reg1, char* _Reg2, char* _Comment)
{
	Address* _InstIndexBase = (Address*) malloc (sizeof(Address)*2); memset(_InstIndexBase, 0, sizeof(Address)*2); _InstIndexBase[0] = _InstIndex; _InstIndexBase[1] = _BinIndex;
	insertCodeToWriteAsm("ESC ", _InstIndex++);
	insertCodeToWriteAsm(_Reg1, _InstIndex++);
	insertCodeToWriteAsm(", ", _InstIndex++);
	insertCodeToWriteAsm(_Reg2, _InstIndex++);

	if(strcmp(_Comment, "") != 0){
		char _sPrint[255];
		sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);
		insertCodeToWriteAsm(_sPrint, _InstIndex++);
	}

	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 4 + 1
	insertCodeToWriteBin(BinaryGen::ESC);
	insertCodeToWriteBin(NULL,_Reg1);
	insertCodeToWriteBin(NULL,_Reg2);

	_Memory->incrementPC(); return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
Address* CodeGeneratorModule::HLT(char* _Comment)
{
	Address* _InstIndexBase = (Address*) malloc (sizeof(Address)*2); memset(_InstIndexBase, 0, sizeof(Address)*2); _InstIndexBase[0] = _InstIndex; _InstIndexBase[1] = _BinIndex;
	insertCodeToWriteAsm("HLT", _InstIndex++);

	if(strcmp(_Comment, "") != 0){
		char _sPrint[255];
		sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);
		insertCodeToWriteAsm(_sPrint, _InstIndex++);
	}

	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 1 + 1
	insertCodeToWriteBin(BinaryGen::HLT);

	_Memory->incrementPC(); return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
Address* CodeGeneratorModule::JMP(char* _Label, char* _Comment)//TODO:Label necessita conversão para o inteiro correspondente
{
	Address* _InstIndexBase = (Address*) malloc (sizeof(Address)*2); memset(_InstIndexBase, 0, sizeof(Address)*2); _InstIndexBase[0] = _InstIndex; _InstIndexBase[1] = _BinIndex;
	insertCodeToWriteAsm("JMP ", _InstIndex++);
	insertCodeToWriteAsm(_Label, _InstIndex++);

	if(strcmp(_Comment, "") != 0){
		char _sPrint[255];
		sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);
		insertCodeToWriteAsm(_sPrint, _InstIndex++);
	}

	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 3 + 1	
	insertCodeToWriteBin(BinaryGen::JMP);
	insertCodeToWriteBin(NULL,_Label);

	_Memory->incrementPC(); return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
Address* CodeGeneratorModule::LDI(char* _RegD, char* _Imed, char* _Comment)
{
	Address* _InstIndexBase = (Address*) malloc (sizeof(Address)*2); memset(_InstIndexBase, 0, sizeof(Address)*2); _InstIndexBase[0] = _InstIndex; _InstIndexBase[1] = _BinIndex;
	insertCodeToWriteAsm("LDI ", _InstIndex++);
	insertCodeToWriteAsm(_RegD, _InstIndex++);	
	insertCodeToWriteAsm(", #", _InstIndex++);
	insertCodeToWriteAsm(_Imed, _InstIndex++);

	if(strcmp(_Comment, "") != 0){
		char _sPrint[255];
		sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);
		insertCodeToWriteAsm(_sPrint, _InstIndex++);
	}

	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 4 + 1
	insertCodeToWriteBin(BinaryGen::LDI);
	insertCodeToWriteBin(NULL,_RegD);
	insertCodeToWriteBin(NULL,_Imed);

	_Memory->incrementPC(); return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
Address* CodeGeneratorModule::LDIF(char* _RegD, char* _Imed, char* _Comment)
{
	Address* _InstIndexBase = (Address*) malloc (sizeof(Address)*2); memset(_InstIndexBase, 0, sizeof(Address)*2); _InstIndexBase[0] = _InstIndex; _InstIndexBase[1] = _BinIndex;
	insertCodeToWriteAsm("LDIF ", _InstIndex++);
	insertCodeToWriteAsm(_RegD, _InstIndex++);
	insertCodeToWriteAsm(", #", _InstIndex++);
	insertCodeToWriteAsm(_Imed, _InstIndex++);

	if(strcmp(_Comment, "") != 0){
		char _sPrint[255];
		sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);
		insertCodeToWriteAsm(_sPrint, _InstIndex++);
	}

	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 4 + 1	
	insertCodeToWriteBin(BinaryGen::LDIF);
	insertCodeToWriteBin(NULL,_RegD);
	insertCodeToWriteBin(NULL,_Imed);

	_Memory->incrementPC(); return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
Address* CodeGeneratorModule::LGT(char* _Reg, char* _Comment)
{
	Address* _InstIndexBase = (Address*) malloc (sizeof(Address)*2); memset(_InstIndexBase, 0, sizeof(Address)*2); _InstIndexBase[0] = _InstIndex; _InstIndexBase[1] = _BinIndex;
	insertCodeToWriteAsm("LGT ", _InstIndex++);
	insertCodeToWriteAsm(_Reg, _InstIndex++);

	if(strcmp(_Comment, "") != 0){
		char _sPrint[255];
		sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);
		insertCodeToWriteAsm(_sPrint, _InstIndex++);
	}

	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 2 + 1
	insertCodeToWriteBin(BinaryGen::LGT);
	insertCodeToWriteBin(NULL,_Reg);

	_Memory->incrementPC(); return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
Address* CodeGeneratorModule::LOD(char* _RegD, Address _Desl, char* _Comment)
{
	Address* _InstIndexBase = (Address*) malloc (sizeof(Address)*2); memset(_InstIndexBase, 0, sizeof(Address)*2); _InstIndexBase[0] = _InstIndex; _InstIndexBase[1] = _BinIndex;
	insertCodeToWriteAsm("LOD ", _InstIndex++);
	insertCodeToWriteAsm(_RegD, _InstIndex++);	
	insertCodeToWriteAsm(", ", _InstIndex++);
	insertCodeToWriteAsm(ATWgetCStr(_Desl), _InstIndex++);

	insertCodeToWriteAsm("(DS)", _InstIndex++);

	if(strcmp(_Comment, "") != 0){
		char _sPrint[255];
		sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);
		insertCodeToWriteAsm(_sPrint, _InstIndex++);
	}

	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 5 + 1
	insertCodeToWriteBin(BinaryGen::LOD);
	insertCodeToWriteBin(NULL,_RegD);
	insertCodeToWriteBin(_Desl);

	_Memory->incrementPC(); return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
Address* CodeGeneratorModule::LODF(char* _RegD, Address _Desl, char* _Comment)
{
	Address* _InstIndexBase = (Address*) malloc (sizeof(Address)*2); memset(_InstIndexBase, 0, sizeof(Address)*2); _InstIndexBase[0] = _InstIndex; _InstIndexBase[1] = _BinIndex;
	insertCodeToWriteAsm("LODF ", _InstIndex++);
	insertCodeToWriteAsm(_RegD, _InstIndex++);	
	insertCodeToWriteAsm(", ", _InstIndex++);
	insertCodeToWriteAsm(ATWgetCStr(_Desl), _InstIndex++);
	insertCodeToWriteAsm("(DS)", _InstIndex++);

	if(strcmp(_Comment, "") != 0){
		char _sPrint[255];
		sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);
		insertCodeToWriteAsm(_sPrint, _InstIndex++);
	}

	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 5 + 1
	insertCodeToWriteBin(BinaryGen::LODF);
	insertCodeToWriteBin(NULL,_RegD);
	insertCodeToWriteBin(_Desl);

	_Memory->incrementPC(); return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
Address* CodeGeneratorModule::MVE(char* _RegD, char* _RegO, char* _Comment)
{
	Address* _InstIndexBase = (Address*) malloc (sizeof(Address)*2); memset(_InstIndexBase, 0, sizeof(Address)*2); _InstIndexBase[0] = _InstIndex; _InstIndexBase[1] = _BinIndex;
	insertCodeToWriteAsm("MVE ", _InstIndex++);
	insertCodeToWriteAsm(_RegD, _InstIndex++);
	insertCodeToWriteAsm(", ", _InstIndex++);
	insertCodeToWriteAsm(_RegO, _InstIndex++);

	if(strcmp(_Comment, "") != 0){
		char _sPrint[255];
		sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);
		insertCodeToWriteAsm(_sPrint, _InstIndex++);
	}

	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 4 + 1
	insertCodeToWriteBin(BinaryGen::MVE);
	insertCodeToWriteBin(NULL,_RegD);
	insertCodeToWriteBin(NULL,_RegO);

	_Memory->incrementPC(); return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
Address* CodeGeneratorModule::MVEF(char* _RegD, char* _RegO, char* _Comment)
{
	Address* _InstIndexBase = (Address*) malloc (sizeof(Address)*2); memset(_InstIndexBase, 0, sizeof(Address)*2); _InstIndexBase[0] = _InstIndex; _InstIndexBase[1] = _BinIndex;
	insertCodeToWriteAsm("MVEF ", _InstIndex++);
	insertCodeToWriteAsm(_RegD, _InstIndex++);
	insertCodeToWriteAsm(", ", _InstIndex++);
	insertCodeToWriteAsm(_RegO, _InstIndex++);

	if(strcmp(_Comment, "") != 0){
		char _sPrint[255];
		sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);
		insertCodeToWriteAsm(_sPrint, _InstIndex++);
	}

	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 4 + 1	
	insertCodeToWriteBin(BinaryGen::MVEF);
	insertCodeToWriteBin(NULL,_RegD);
	insertCodeToWriteBin(NULL,_RegO);

	_Memory->incrementPC(); return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
Address* CodeGeneratorModule::MUL(char* _RegD, char* _RegO, char* _Comment)
{
	Address* _InstIndexBase = (Address*) malloc (sizeof(Address)*2); memset(_InstIndexBase, 0, sizeof(Address)*2); _InstIndexBase[0] = _InstIndex; _InstIndexBase[1] = _BinIndex;
	insertCodeToWriteAsm("MUL ", _InstIndex++);
	insertCodeToWriteAsm(_RegD, _InstIndex++);
	insertCodeToWriteAsm(", ", _InstIndex++);
	insertCodeToWriteAsm(_RegO, _InstIndex++);

	if(strcmp(_Comment, "") != 0){
		char _sPrint[255];
		sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);
		insertCodeToWriteAsm(_sPrint, _InstIndex++);
	}

	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 4 + 1	
	insertCodeToWriteBin(BinaryGen::MUL);
	insertCodeToWriteBin(NULL,_RegD);
	insertCodeToWriteBin(NULL,_RegO);

	_Memory->incrementPC(); return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
Address* CodeGeneratorModule::MULF(char* _RegD, char* _RegO, char* _Comment)
{
	Address* _InstIndexBase = (Address*) malloc (sizeof(Address)*2); memset(_InstIndexBase, 0, sizeof(Address)*2); _InstIndexBase[0] = _InstIndex; _InstIndexBase[1] = _BinIndex;
	insertCodeToWriteAsm("MULF ", _InstIndex++);
	insertCodeToWriteAsm(_RegD, _InstIndex++);
	insertCodeToWriteAsm(", ", _InstIndex++);
	insertCodeToWriteAsm(_RegO, _InstIndex++);

	if(strcmp(_Comment, "") != 0){
		char _sPrint[255];
		sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);
		insertCodeToWriteAsm(_sPrint, _InstIndex++);
	}

	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 4 + 1
	insertCodeToWriteBin(BinaryGen::MULF);
	insertCodeToWriteBin(NULL,_RegD);
	insertCodeToWriteBin(NULL,_RegO);

	_Memory->incrementPC(); return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
Address* CodeGeneratorModule::NEG(char* _Reg, char* _Comment)
{
	Address* _InstIndexBase = (Address*) malloc (sizeof(Address)*2); memset(_InstIndexBase, 0, sizeof(Address)*2); _InstIndexBase[0] = _InstIndex; _InstIndexBase[1] = _BinIndex;
	insertCodeToWriteAsm("NEG ", _InstIndex++);
	insertCodeToWriteAsm(_Reg, _InstIndex++);	

	if(strcmp(_Comment, "") != 0){
		char _sPrint[255];
		sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);
		insertCodeToWriteAsm(_sPrint, _InstIndex++);
	}

	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 2 + 1	
	insertCodeToWriteBin(BinaryGen::NEG);
	insertCodeToWriteBin(NULL,_Reg);

	_Memory->incrementPC(); return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
Address* CodeGeneratorModule::NEGF(char* _Reg, char* _Comment)
{
	Address* _InstIndexBase = (Address*) malloc (sizeof(Address)*2); memset(_InstIndexBase, 0, sizeof(Address)*2); _InstIndexBase[0] = _InstIndex; _InstIndexBase[1] = _BinIndex;
	insertCodeToWriteAsm("NEGF ", _InstIndex++);
	insertCodeToWriteAsm(_Reg, _InstIndex++);	

	if(strcmp(_Comment, "") != 0){
		char _sPrint[255];
		sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);
		insertCodeToWriteAsm(_sPrint, _InstIndex++);
	}

	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 2 + 1	
	insertCodeToWriteBin(BinaryGen::NEGF);
	insertCodeToWriteBin(NULL,_Reg);

	_Memory->incrementPC(); return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
Address* CodeGeneratorModule::RTR(char* _Comment)
{
	Address* _InstIndexBase = (Address*) malloc (sizeof(Address)*2); memset(_InstIndexBase, 0, sizeof(Address)*2); _InstIndexBase[0] = _InstIndex; _InstIndexBase[1] = _BinIndex;
	insertCodeToWriteAsm("RTR", _InstIndex++);

	if(strcmp(_Comment, "") != 0){
		char _sPrint[255];
		sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);
		insertCodeToWriteAsm(_sPrint, _InstIndex++);
	}

	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize =
	insertCodeToWriteBin(BinaryGen::RTR);

	_Memory->incrementPC(); return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
Address* CodeGeneratorModule::STI(char* _Imed, Address _Desl, char* _Comment){
	Address* _InstIndexBase = (Address*) malloc (sizeof(Address)*2); memset(_InstIndexBase, 0, sizeof(Address)*2); _InstIndexBase[0] = _InstIndex; _InstIndexBase[1] = _BinIndex;

	insertCodeToWriteAsm("STI #", _InstIndex++);
	insertCodeToWriteAsm(_Imed, _InstIndex++);
	insertCodeToWriteAsm(", ", _InstIndex++);
	insertCodeToWriteAsm(ATWgetCStr(_Desl), _InstIndex++);
	insertCodeToWriteAsm("(DS)", _InstIndex++);

	if(strcmp(_Comment, "") != 0){
		char _sPrint[255];
		sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);
		insertCodeToWriteAsm(_sPrint, _InstIndex++);
	}

	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 5 + 1
	insertCodeToWriteBin(BinaryGen::STI);
	insertCodeToWriteBin(NULL, _Imed);
	insertCodeToWriteBin(_Desl);

	_Memory->incrementPC(); return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
Address* CodeGeneratorModule::STIF(char* _Imed, Address _Desl, char* _Comment)
{
	Address* _InstIndexBase = (Address*) malloc (sizeof(Address)*2); memset(_InstIndexBase, 0, sizeof(Address)*2); _InstIndexBase[0] = _InstIndex; _InstIndexBase[1] = _BinIndex;
	insertCodeToWriteAsm("STIF #", _InstIndex++);
	insertCodeToWriteAsm(_Imed, _InstIndex++);
	insertCodeToWriteAsm(", ", _InstIndex++);
	insertCodeToWriteAsm(ATWgetCStr(_Desl), _InstIndex++);
	insertCodeToWriteAsm("(DS)", _InstIndex++);

	if(strcmp(_Comment, "") != 0){
		char _sPrint[255];
		sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);
		insertCodeToWriteAsm(_sPrint, _InstIndex++);
	}

	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 5 + 1
	insertCodeToWriteBin(BinaryGen::STIF);
	insertCodeToWriteBin(NULL,_Imed);
	insertCodeToWriteBin(_Desl);

	_Memory->incrementPC(); return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
Address* CodeGeneratorModule::STO(char* _Reg, Address _Desl, char* _Comment)
{
	Address* _InstIndexBase = (Address*) malloc (sizeof(Address)*2); memset(_InstIndexBase, 0, sizeof(Address)*2); _InstIndexBase[0] = _InstIndex; _InstIndexBase[1] = _BinIndex;
	insertCodeToWriteAsm("STO ", _InstIndex++);
	insertCodeToWriteAsm(_Reg, _InstIndex++);
	insertCodeToWriteAsm(", ", _InstIndex++);
	insertCodeToWriteAsm(ATWgetCStr(_Desl), _InstIndex++);

	insertCodeToWriteAsm("(DS)", _InstIndex++);

	if(strcmp(_Comment, "") != 0){
		char _sPrint[255];
		sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);
		insertCodeToWriteAsm(_sPrint, _InstIndex++);
	}

	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 5 + 1
	insertCodeToWriteBin(BinaryGen::STO);
	insertCodeToWriteBin(NULL,_Reg);
	insertCodeToWriteBin(_Desl);

	_Memory->incrementPC(); return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
Address* CodeGeneratorModule::STOF(char* _Reg, Address _Desl, char* _Comment)
{
	Address* _InstIndexBase = (Address*) malloc (sizeof(Address)*2); memset(_InstIndexBase, 0, sizeof(Address)*2); _InstIndexBase[0] = _InstIndex; _InstIndexBase[1] = _BinIndex;
	insertCodeToWriteAsm("STOF ", _InstIndex++);
	insertCodeToWriteAsm(_Reg, _InstIndex++);
	insertCodeToWriteAsm(", ", _InstIndex++);
	insertCodeToWriteAsm(ATWgetCStr(_Desl), _InstIndex++);
	insertCodeToWriteAsm("(DS)", _InstIndex++);

	if(strcmp(_Comment, "") != 0){
		char _sPrint[255];
		sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);
		insertCodeToWriteAsm(_sPrint, _InstIndex++);
	}

	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 5 + 1
	insertCodeToWriteBin(BinaryGen::STOF);
	insertCodeToWriteBin(NULL,_Reg);
	insertCodeToWriteBin(_Desl);

	_Memory->incrementPC(); return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
Address* CodeGeneratorModule::SUB(char* _RegD, char* _RegO, char* _Comment)
{
	Address* _InstIndexBase = (Address*) malloc (sizeof(Address)*2); memset(_InstIndexBase, 0, sizeof(Address)*2); _InstIndexBase[0] = _InstIndex; _InstIndexBase[1] = _BinIndex;
	insertCodeToWriteAsm("SUB ", _InstIndex++);
	insertCodeToWriteAsm(_RegD, _InstIndex++);
	insertCodeToWriteAsm(", ", _InstIndex++);
	insertCodeToWriteAsm(_RegO, _InstIndex++);

	if(strcmp(_Comment, "") != 0){
		char _sPrint[255];
		sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);
		insertCodeToWriteAsm(_sPrint, _InstIndex++);
	}

	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 4 + 1
	insertCodeToWriteBin(BinaryGen::SUB);
	insertCodeToWriteBin(NULL,_RegD);
	insertCodeToWriteBin(NULL,_RegO);

	_Memory->incrementPC(); return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
Address* CodeGeneratorModule::SUBF(char* _RegD, char* _RegO, char* _Comment)
{
	Address* _InstIndexBase = (Address*) malloc (sizeof(Address)*2); memset(_InstIndexBase, 0, sizeof(Address)*2); _InstIndexBase[0] = _InstIndex; _InstIndexBase[1] = _BinIndex;
	insertCodeToWriteAsm("SUBF ", _InstIndex++);
	insertCodeToWriteAsm(_RegD, _InstIndex++);
	insertCodeToWriteAsm(", ", _InstIndex++);
	insertCodeToWriteAsm(_RegO, _InstIndex++);

	if(strcmp(_Comment, "") != 0){
		char _sPrint[255];
		sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);
		insertCodeToWriteAsm(_sPrint, _InstIndex++);
	}

	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 4 + 1
	insertCodeToWriteBin(BinaryGen::SUBF);
	insertCodeToWriteBin(NULL,_RegD);
	insertCodeToWriteBin(NULL,_RegO);

	_Memory->incrementPC(); return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------
Address* CodeGeneratorModule::TME(char* _Reg, char* _Comment)
{
	Address* _InstIndexBase = (Address*) malloc (sizeof(Address)*2); memset(_InstIndexBase, 0, sizeof(Address)*2); _InstIndexBase[0] = _InstIndex; _InstIndexBase[1] = _BinIndex;
	insertCodeToWriteAsm("TME ", _InstIndex++);
	insertCodeToWriteAsm(_Reg, _InstIndex++);

	if(strcmp(_Comment, "") != 0){
		char _sPrint[255];
		sprintf_s(_sPrint, " ATW_COMMENT: %s", _Comment);
		insertCodeToWriteAsm(_sPrint, _InstIndex++);
	}

	insertCodeToWriteAsm("\n", _InstIndex++);

	//BlockSize = 2 + 1
	insertCodeToWriteBin(BinaryGen::TME);
	insertCodeToWriteBin(NULL,_Reg);

	_Memory->incrementPC(); return _InstIndexBase;
}
//----------------------------------------------------------------------------------------------------------------------