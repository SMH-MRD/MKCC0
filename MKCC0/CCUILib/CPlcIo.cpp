#include "pch.h"
#include "CPlcIo.h"

//CNotchIO
HRESULT CNotchIO::write_io(INT16 val) {
	if (_pwbuf == NULL) return S_FALSE;
	set_w(val);	
	INT16 i = _wval_plc + PLCIO_0_NOTCH_INDEX;
	if ((i >= 0) && (_wval_plc < PLCIO_N_NOTCH_INDEX)) {
		*_pwbuf &= ~bit_mask_w;
		*_pwbuf |= o_ptn[i];
		return S_OK;	
	}
	else return S_FALSE;
}
INT16 CNotchIO::read_io() {		//dispの内容をIOバッファから読み出し
	if (_prbuf == NULL) return S_FALSE;

	INT16 notch_read = *_prbuf & bit_mask_r;
	for (int i = 0; i < PLCIO_N_NOTCH_INDEX; i++) {
		if (notch_read == i_ptn[i]) {
			_rval_plc = i - PLCIO_0_NOTCH_INDEX;
			return _rval_plc;
		}
	}
	return 0;
}

//CModeIO
HRESULT CModeIO::set_w(INT16 val){
	//モードの設定値は2の階乗値とする
	int count = 0;
	INT16 bit_check = 1;
	for (int i = 0; i < nbit_w; i++) {	//bitチェック
		if (val & bit_check)	count++;
		if (count > 1) {					//1以上はエラー
			_wval_plc = -1; return S_FALSE;
		}
		_wval_plc = bit_check;
		bit_check <<= 1;
	}
	_wval_plc = -1; return S_FALSE;
}

HRESULT CModeIO::write_io(INT16 val) {
	if (_pwbuf == NULL) return S_FALSE;

	this->set_w(val);	
	
	INT16 setval = _wval_plc;
	setval <<= ibit0_w;
	setval &= bit_mask_w;
	*_pwbuf |= setval;

	return S_OK;
}
INT16 CModeIO::read_io(){
	if (_prbuf == NULL) return S_FALSE;

	_rval_plc = (*_prbuf &= bit_mask_r);
	return _rval_plc >>= ibit0_r;
}