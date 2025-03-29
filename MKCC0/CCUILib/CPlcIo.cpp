#include "pch.h"
#include "CPlcIo.h"

//CNotchIO
void CNotchIO::set_disp(UN_PLC_DATA _val) {
	if ((_val.i16 > PLCIO_0_NOTCH_INDEX) || (_val.i16 < -PLCIO_0_NOTCH_INDEX)) {
		_disp_val.i16 = 0;
	}
	else {
		_disp_val.i16 = _val.i16;
	}
	return;
}
void CNotchIO::write_io() {    //disp_valの内容をIOバッファに出力
	*_pwbuf = io_ptn[_disp_val.i16 + PLCIO_0_NOTCH_INDEX]; return;
}
void CNotchIO::write_io(UN_PLC_DATA _val) {
	set_disp(_val);	write_io(); return;
}
UN_PLC_DATA CNotchIO::read_io() {		//dispの内容をIOバッファから読み出し
	_io_val.i16 = *_prbuf;
	for (int i = 0; i < PLCIO_N_NOTCH_INDEX; i++) {
		if (*_prbuf == io_ptn[i]) {
			_io_val.i16 = i - PLCIO_0_NOTCH_INDEX;
			return _io_val;
		}
	}
	_io_val.i16 = 0;
	return _io_val;
}

//CWordIO
void CWordIO::set_disp(UN_PLC_DATA _val) { 
	_disp_val = _val; return;
}
void CWordIO::write_io() {
	switch (_param) {
	case PLCIO_CODE_INT16 : { *_pwbuf = _disp_val.i16; }break;
	case PLCIO_CODE_UINT16: { *((PUINT16)_pwbuf) = _disp_val.ui16;}break;
	case PLCIO_CODE_INT32 : { *((PINT32)_pwbuf) = _disp_val.i32; }break;
	case PLCIO_CODE_UINT32: { *((PUINT32)_pwbuf) = _disp_val.ui32; }break;
	case PLCIO_CODE_FLOAT : { *((PFLOAT)_pwbuf) = _disp_val.f; }break;
	case PLCIO_CODE_DOUBLE: { *((double*)_pwbuf) = _disp_val.d; }break;
	default:break;
	}return;
}
void CWordIO::write_io(UN_PLC_DATA _val) {
	set_disp(_val);	write_io(); return;
}
UN_PLC_DATA CWordIO::read_io() {
	switch (_param) {
	case PLCIO_CODE_INT16: { _io_val.i16	= _disp_val.i16	= *_prbuf;				}break;
	case PLCIO_CODE_UINT16:{ _io_val.ui16	= _disp_val.ui16= *((PUINT16)_prbuf);	}break;
	case PLCIO_CODE_INT32: { _io_val.i32	= _disp_val.i32	= *((PINT32)_prbuf);		}break;
	case PLCIO_CODE_UINT32:{ _io_val.ui32	= _disp_val.ui32= *((PUINT32)_prbuf);	}break;
	case PLCIO_CODE_FLOAT: { _io_val.f		= _disp_val.f	= *((PFLOAT)_prbuf);		}break;
	case PLCIO_CODE_DOUBLE:{ _io_val.d		= _disp_val.d	= *((double*)_prbuf);	}break;
	default:break;
	}
	return _io_val;
}

//CModeIO
void CModeIO::set_disp(UN_PLC_DATA _val){
	if ((_val.i16 < 0) || (_val.i16 >= (2 ^ nbit))) {
		_disp_val.i16 = -1;
	}
	else {
		int count = 0;
		for (int i = 0; i < nbit; i++) {	//bit数チェック
			if(_val.i16 & bit_ptn[i])count++;
		}
		if (count > 1) {					//1以上はエラー
			_disp_val.i16 = -1; return;
		}
		_disp_val.i16 = _val.i16;
	}
	return;
}
void CModeIO::write_io(){

	INT16 setval = _disp_val.i16;
	setval <<= ibit0;
	*_pwbuf &= ~_param;
	*_pwbuf |= setval; 
	return;
}
void CModeIO::write_io(UN_PLC_DATA _val) {
	set_disp(_val);	write_io(); return;
}
UN_PLC_DATA CModeIO::read_io(){
	INT16 readval = (*_prbuf & ~_param);
	_io_val.i16 = readval >> ibit0;
	return _io_val;
}