#include "pch.h"
#include "CPlcIo.h"

//CNotchIO
HRESULT CNotchIO::write_io(INT16 val) {//val　ノッチ値(-5～5)
	if (_pwbuf == NULL) return S_FALSE;
	set_w(val);										//_wval_plcにセット
	INT16 i = _wval_plc + PLCIO_0_NOTCH_INDEX;		//ノッチパターン配列の対応インデックス
	if ((i >= 0) && (i < PLCIO_N_NOTCH_INDEX)) {	//範囲チェック
		*_pwbuf &= ~bit_mask_w;						//マスク範囲をクリア
		*_pwbuf |= o_ptn[i];						//ノッチパターンをセット
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

//ノッチパターンのセットアップ
//実機IO割付に応じてビットシフトする
//シフト量はマスクパラメータから取得する
HRESULT CNotchIO::setup_notch_ptn() {	//ノッチパターンのセット
	if (bit_mask_w == 0 || bit_mask_r == 0) return S_FALSE;
	int n_shift_i = 0, n_shift_o = 0;

	for (int i = 0; i <= (16 - PLCIO_N_PTN_BIT); i++) {
		if (bit_mask_r & (1 << i)) {
			n_shift_i = i;
			break;
		}
	}
	for (int i = 0; i < PLCIO_N_NOTCH_INDEX; i++) {
		i_ptn[i] <<= n_shift_i;
	}

	for (int i = 0; i <= (16 - PLCIO_N_PTN_BIT); i++) {
		if (bit_mask_w & (1 << i)) {
			n_shift_o = i;
			break;
		}
	}
	for (int i = 0; i < PLCIO_N_NOTCH_INDEX; i++) {
		o_ptn[i] <<= n_shift_o;
	}
	return S_OK;
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