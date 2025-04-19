#include "pch.h"
#include "CPlcIo.h"

//CNotchIO
HRESULT CNotchIO::write_io(INT16 val) {//val�@�m�b�`�l(-5�`5)
	if (_pwbuf == NULL) return S_FALSE;
	set_w(val);										//_wval_plc�ɃZ�b�g
	INT16 i = _wval_plc + PLCIO_0_NOTCH_INDEX;		//�m�b�`�p�^�[���z��̑Ή��C���f�b�N�X
	if ((i >= 0) && (i < PLCIO_N_NOTCH_INDEX)) {	//�͈̓`�F�b�N
		*_pwbuf &= ~bit_mask_w;						//�}�X�N�͈͂��N���A
		*_pwbuf |= o_ptn[i];						//�m�b�`�p�^�[�����Z�b�g
		return S_OK;	
	}
	else return S_FALSE;
}
INT16 CNotchIO::read_io() {		//disp�̓��e��IO�o�b�t�@����ǂݏo��
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

//�m�b�`�p�^�[���̃Z�b�g�A�b�v
//���@IO���t�ɉ����ăr�b�g�V�t�g����
//�V�t�g�ʂ̓}�X�N�p�����[�^����擾����
HRESULT CNotchIO::setup_notch_ptn() {	//�m�b�`�p�^�[���̃Z�b�g
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
	//���[�h�̐ݒ�l��2�̊K��l�Ƃ���
	int count = 0;
	INT16 bit_check = 1;
	for (int i = 0; i < nbit_w; i++) {	//bit�`�F�b�N
		if (val & bit_check)	count++;
		if (count > 1) {					//1�ȏ�̓G���[
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