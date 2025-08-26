#include <stdexcept>
#include "CMCProtocol.h"
#include "CSockLib.h"

static CSockUDP* pMCSock;

//�O���[�o��
LARGE_INTEGER frequency;                //�V�X�e���̎��g��
LARGE_INTEGER start_count_w, end_count_w, start_count_r, end_count_r;   //�V�X�e���J�E���g��
LONGLONG spancount_max_w, spancount_max_r;

//*********************************************************************************************�@
/// <summary>
/// �R���X�g���N�^
/// </summary>
CMCProtocol::CMCProtocol(INT32 _eventID) {
	eventID = _eventID;
	D_no_r = D_no_w = n_D_read = n_D_write = 0;
	memset(&rcv_buf, 0, sizeof(rcv_buf));
	memset(&snd_buf, 0, sizeof(snd_buf));

}
//*********************************************************************************************�@
/// <summary>
/// �f�X�g���N�^
/// </summary>
CMCProtocol::~CMCProtocol() {
	delete pMCSock;
	close();
}
//*********************************************************************************************�@
/// <summary>
/// �w��\�P�b�g�����A�ǂݏ����f�o�C�X�w��
/// </summary>
/// <param name="hWnd">�\�P�b�g�C�x���g����M����E�B���h�E�̃n���h��</param>
/// <param name="paddrinc">	��M�\�P�b�g�A�h���X</param>
/// <param name="paddrins">	���M�\�P�b�g�A�h���X</param>
/// <param name="no_r">		�ǂݏo��D�f�o�C�X�擪�ԍ�</param>
/// <param name="num_r">	�ǂݏo��D�f�o�C�X��</param>
/// <param name="no_w">		��������D�f�o�C�X�擪�ԍ�</param>
/// <param name="num_w">	��������D�f�o�C�X����</param>
/// <returns></returns>
HRESULT CMCProtocol::Initialize(HWND hwnd, int type) {
	switch (type) {
	case PLC_IF_TYPE_SLBRK: //SLBRK IF
	{
		//�ǂݏ����f�o�C�X�ݒ�
		set_access_D_r(SLBRK_MC_ADDR_W_READ, SLBRK_MC_SIZE_W_READ);  //�ǂݏo��D�f�o�C�X�擪�A�h���X�Z�b�g
		set_access_D_w(SLBRK_MC_ADDR_W_WRITE, SLBRK_MC_SIZE_W_WRITE);//��������D�f�o�C�X�擪�A�h���X�Z�b�g

		//���M�o�b�t�@�t�H�[�}�b�g�i�w�b�_���j�ݒ�
		set_sndbuf_read_D_3E();		//3E�@D�f�o�C�X�ǂݍ��ݗp�v�����M�t�H�[�}�b�g�Z�b�g
		set_sndbuf_write_D_3E();	//3E�@D�f�o�C�X�������ݗp�v�����M�t�H�[�}�b�g�Z�b�g

		//�N���C�A���gUDP�\�P�b�g�@�A�h���X�ݒ�@�C���X�^���X���@������

		pMCSock = new CSockUDP(ACCESS_TYPE_CLIENT, eventID);
		if (pMCSock->Initialize() != S_OK) {							//��M�\�P�b�g����
			msg_wos.str() = pMCSock->err_msg.str();
			return S_FALSE;
		}
		else {
			pMCSock->set_sock_addr(&pMCSock->addr_in_rcv, IP_ADDR_MC_CLIENT_SLBRK, PORT_MC_CLIENT_SLBRK);
			if (pMCSock->init_sock(hwnd, pMCSock->addr_in_rcv) != S_OK) {//��M�\�P�b�g�ݒ�
				msg_wos.str() = pMCSock->err_msg.str();
				return S_FALSE;
			}
		}

		//���M��A�h���X�Z�b�g
		pMCSock->set_sock_addr(&pMCSock->addr_in_dst, IP_ADDR_MC_SERVER_SLBRK, PORT_MC_SERVER_SLBRK);
	}break;
	case PLC_IF_TYPE_OTE: //OTE IF
	{
		//�ǂݏ����f�o�C�X�ݒ�
		set_access_D_r(OTE_MC_ADDR_W_READ, OTE_MC_SIZE_W_READ);  //�ǂݏo��D�f�o�C�X�擪�A�h���X�Z�b�g
		set_access_D_w(OTE_MC_ADDR_W_WRITE, OTE_MC_SIZE_W_WRITE);//��������D�f�o�C�X�擪�A�h���X�Z�b�g

		//���M�o�b�t�@�t�H�[�}�b�g�i�w�b�_���j�ݒ�
		set_sndbuf_read_D_3E();		//3E�@D�f�o�C�X�ǂݍ��ݗp�v�����M�t�H�[�}�b�g�Z�b�g
		set_sndbuf_write_D_3E();	//3E�@D�f�o�C�X�������ݗp�v�����M�t�H�[�}�b�g�Z�b�g

		//�N���C�A���gUDP�\�P�b�g�@�A�h���X�ݒ�@�C���X�^���X���@������

		pMCSock = new CSockUDP(ACCESS_TYPE_CLIENT, eventID);
		if (pMCSock->Initialize() != S_OK) {							//��M�\�P�b�g����
			msg_wos.str() = pMCSock->err_msg.str();
			return S_FALSE;
		}
		else {
			pMCSock->set_sock_addr(&pMCSock->addr_in_rcv, IP_ADDR_MC_CLIENT_OTE, PORT_MC_CLIENT_OTE);
			if (pMCSock->init_sock(hwnd, pMCSock->addr_in_rcv) != S_OK) {//��M�\�P�b�g�ݒ�
				msg_wos.str() = pMCSock->err_msg.str();
				return S_FALSE;
			}
		}

		//���M��A�h���X�Z�b�g
		pMCSock->set_sock_addr(&pMCSock->addr_in_dst, IP_ADDR_MC_SERVER_OTE, PORT_MC_SERVER_OTE);
	}break;
	case PLC_IF_TYPE_CC: //CC IF
	default:
	{
		//�ǂݏ����f�o�C�X�ݒ�
		set_access_D_r(CC_MC_ADDR_W_READ, CC_MC_SIZE_W_READ);  //�ǂݏo��D�f�o�C�X�擪�A�h���X�Z�b�g
		set_access_D_w(CC_MC_ADDR_W_WRITE, CC_MC_SIZE_W_WRITE);//��������D�f�o�C�X�擪�A�h���X�Z�b�g

		//���M�o�b�t�@�t�H�[�}�b�g�i�w�b�_���j�ݒ�
		set_sndbuf_read_D_3E();		//3E�@D�f�o�C�X�ǂݍ��ݗp�v�����M�t�H�[�}�b�g�Z�b�g
		set_sndbuf_write_D_3E();	//3E�@D�f�o�C�X�������ݗp�v�����M�t�H�[�}�b�g�Z�b�g

		//�N���C�A���gUDP�\�P�b�g�@�A�h���X�ݒ�@�C���X�^���X���@������

		pMCSock = new CSockUDP(ACCESS_TYPE_CLIENT, eventID);
		if (pMCSock->Initialize() != S_OK) {									//��M�\�P�b�g����
			msg_wos.str() = pMCSock->err_msg.str();
			return S_FALSE;
		}
		else {
			pMCSock->set_sock_addr(&pMCSock->addr_in_rcv, IP_ADDR_MC_CLIENT, PORT_MC_CLIENT);
			if (pMCSock->init_sock(hwnd, pMCSock->addr_in_rcv) != S_OK) {//��M�\�P�b�g�ݒ�
				msg_wos.str() = pMCSock->err_msg.str();
				return S_FALSE;
			}
		}

		//���M��A�h���X�Z�b�g
		pMCSock->set_sock_addr(&pMCSock->addr_in_dst, IP_ADDR_MC_SERVER, PORT_MC_SERVER);
	}
	}
	return S_OK;
}
//**********************************************************************************************�@
/// <summary>
/// �N���[�Y����
/// </summary>
/// <returns></returns>
HRESULT CMCProtocol::close() {

	delete pMCSock;

	return 0;
}
//**********************************************************************************************�@
/// <summary>
/// 3E�t�H�[�}�b�g W�f�o�C�X�ǂݍ��ݗp���M�o�b�t�@�Z�b�g
/// 
/// </summary>
/// <param name="d_no"></param>
/// <param name="n_read"></param>
/// <returns></returns>
HRESULT CMCProtocol::set_sndbuf_read_D_3E() {
	UINT8* p8 = read_req_snd_buf;
	int size, len = 0;
	mc_req_msg_r.subcode = CODE_3E_FORMAT;
	memcpy(p8, &mc_req_msg_r.subcode, size = sizeof(mc_req_msg_r.subcode));	len += size; p8 += size;

	mc_req_msg_r.nNW = CODE_NW;
	memcpy(p8, &mc_req_msg_r.nNW, size = sizeof(mc_req_msg_r.nNW));			len += size; p8 += size;

	mc_req_msg_r.nPC = CODE_PC;
	memcpy(p8, &mc_req_msg_r.nPC, size = sizeof(mc_req_msg_r.nPC));			len += size; p8 += size;

	mc_req_msg_r.nUIO = CODE_UIO;
	memcpy(p8, &mc_req_msg_r.nUIO, size = sizeof(mc_req_msg_r.nUIO));	len += size; p8 += size;

	mc_req_msg_r.nUcode = CODE_UNODE;
	memcpy(p8, &mc_req_msg_r.nUcode, size = sizeof(mc_req_msg_r.nUcode));	len += size; p8 += size;

	mc_req_msg_r.len = 12; 	//�ȉ��̃o�C�g��
	memcpy(p8, &mc_req_msg_r.len, size = sizeof(mc_req_msg_r.len));			len += size; p8 += size;

	mc_req_msg_r.timer = CODE_MON_TIMER;
	memcpy(p8, &mc_req_msg_r.timer, size = sizeof(mc_req_msg_r.timer));		len += size; p8 += size;

	mc_req_msg_r.com = CODE_COMMAND_READ;
	memcpy(p8, &mc_req_msg_r.com, size = sizeof(mc_req_msg_r.com));			len += size; p8 += size;

	mc_req_msg_r.scom = CODE_SUBCOM_READ;
	memcpy(p8, &mc_req_msg_r.scom, size = sizeof(mc_req_msg_r.scom));		len += size; p8 += size;

	mc_req_msg_r.d_no = D_no_r;
	memcpy(p8, &mc_req_msg_r.d_no, size = sizeof(mc_req_msg_r.d_no));		len += size; p8 += size;

	mc_req_msg_r.d_no0 = CODE_3E_NULL;	//�ȒP�̂��ߏ�ʂPbyte�͎g��Ȃ�
	memcpy(p8, &mc_req_msg_r.d_no0, size = sizeof(mc_req_msg_r.d_no0));		len += size; p8 += size;

	mc_req_msg_r.d_code = CODE_DEVICE_D;
	memcpy(p8, &mc_req_msg_r.d_code, size = sizeof(mc_req_msg_r.d_code));	len += size; p8 += size;

	mc_req_msg_r.n_device = n_D_read;
	memcpy(p8, &mc_req_msg_r.n_device, size = sizeof(mc_req_msg_r.n_device)); len += size; p8 += size;

	return S_OK;
}
//**********************************************************************************************�@
/// <summary>
/// //3E�t�H�[�}�b�g W�f�o�C�X�������ݗp���M�o�b�t�@�w�b�_���Z�b�g
/// </summary>
/// <param name="d_no"></param>
/// <param name="n_write"></param>
/// <returns></returns>
HRESULT CMCProtocol::set_sndbuf_write_D_3E() {
	UINT8* p8 = write_req_snd_buf;
	int size, len = 0;
	mc_req_msg_w.subcode = CODE_3E_FORMAT;
	memcpy(p8, &mc_req_msg_w.subcode, size = sizeof(mc_req_msg_w.subcode));	len += size; p8 += size;

	mc_req_msg_w.nNW = CODE_NW;
	memcpy(p8, &mc_req_msg_w.nNW, size = sizeof(mc_req_msg_w.nNW));			len += size; p8 += size;

	mc_req_msg_w.nPC = CODE_PC;
	memcpy(p8, &mc_req_msg_w.nPC, size = sizeof(mc_req_msg_w.nPC));			len += size; p8 += size;

	mc_req_msg_w.nUIO = CODE_UIO;
	memcpy(p8, &mc_req_msg_w.nUIO, size = sizeof(mc_req_msg_w.nUIO));		len += size; p8 += size;

	mc_req_msg_w.nUcode = CODE_UNODE;
	memcpy(p8, &mc_req_msg_w.nUcode, size = sizeof(mc_req_msg_w.nUcode));	len += size; p8 += size;


	mc_req_msg_w.len = 12 + n_D_write * 2; 	//�ȉ��̃o�C�g��
	memcpy(p8, &mc_req_msg_w.len, size = sizeof(mc_req_msg_w.len));			len += size; p8 += size;

	mc_req_msg_w.timer = CODE_MON_TIMER;
	memcpy(p8, &mc_req_msg_w.timer, size = sizeof(mc_req_msg_w.timer));		len += size; p8 += size;

	mc_req_msg_w.com = CODE_COMMAND_WRITE;
	memcpy(p8, &mc_req_msg_w.com, size = sizeof(mc_req_msg_w.com));			len += size; p8 += size;

	mc_req_msg_w.scom = CODE_SUBCOM_WRITE;
	memcpy(p8, &mc_req_msg_w.scom, size = sizeof(mc_req_msg_w.scom));		len += size; p8 += size;

	mc_req_msg_w.d_no = D_no_w;
	memcpy(p8, &mc_req_msg_w.d_no, size = sizeof(mc_req_msg_w.d_no));		len += size; p8 += size;

	mc_req_msg_w.d_no0 = CODE_3E_NULL;//�ȒP�̂��ߏ�ʂPbyte�͎g��Ȃ�
	memcpy(p8, &mc_req_msg_w.d_no0, size = sizeof(mc_req_msg_w.d_no0));		len += size; p8 += size;

	mc_req_msg_w.d_code = CODE_DEVICE_D;
	memcpy(p8, &mc_req_msg_w.d_code, size = sizeof(mc_req_msg_w.d_code));	len += size; p8 += size;

	mc_req_msg_w.n_device = n_D_write;
	memcpy(p8, &mc_req_msg_w.n_device, size = sizeof(mc_req_msg_w.n_device)); len += size; p8 += size;

	for (int i = 0; i < n_D_write; i++) *(p8 + i) = 0;

	return S_OK;
}

//**********************************************************************************************�@
/// <summary>
///	3E�t�H�[�}�b�g W�f�o�C�X�������ݗv�����M
/// </summary>
/// <param name="p_data"></param>
/// <param name="n_write"></param>
/// <returns></returns>
HRESULT CMCProtocol::send_write_req_D_3E(void* p_data) {

	UINT8* p8 = write_req_snd_buf;
	int len = 0, size = 0;
	//�w�b�_�����̏������ݖ���
	size = sizeof(mc_req_msg_w.subcode);	len += size;
	size = sizeof(mc_req_msg_w.nNW);		len += size;
	size = sizeof(mc_req_msg_w.nPC);		len += size;
	size = sizeof(mc_req_msg_w.nUIO);		len += size;
	size = sizeof(mc_req_msg_w.nUcode);		len += size;
	size = sizeof(mc_req_msg_w.len);		len += size;
	size = sizeof(mc_req_msg_w.timer);		len += size;
	size = sizeof(mc_req_msg_w.com);		len += size;
	size = sizeof(mc_req_msg_w.scom);		len += size;
	size = sizeof(mc_req_msg_w.d_no);		len += size;
	size = sizeof(mc_req_msg_w.d_no0);		len += size;
	size = sizeof(mc_req_msg_w.d_code);		len += size;
	size = sizeof(mc_req_msg_w.n_device);	len += size;
	//len = 21;					
	//�f�[�^���̃f�[�^�X�V
	memcpy(p8 + len, p_data, size = n_D_write * 2); len += size;

	//	���M�o�b�t�@�̃T�[�o�[�A�h���X�ɑ��M 
	if (pMCSock->snd_msg((char*)write_req_snd_buf, len, pMCSock->addr_in_dst) == SOCKET_ERROR) {
		msg_wos.str() = pMCSock->err_msg.str();
		return S_FALSE;
	}
	return S_OK;
}
//**********************************************************************************************�@
/// <summary>
/// 3E�t�H�[�}�b�g W�f�o�C�X�ǂݍ��ݗv�����M
/// </summary>
/// <param name="d_no"></param>
/// <param name="n_read"></param>
/// <returns></returns>
HRESULT CMCProtocol::send_read_req_D_3E() {

	int size, len = 0;
	size = sizeof(mc_req_msg_r.subcode); len += size;
	size = sizeof(mc_req_msg_r.nNW);	len += size;
	size = sizeof(mc_req_msg_r.nPC);	len += size;
	size = sizeof(mc_req_msg_r.nUIO);	len += size;
	size = sizeof(mc_req_msg_r.nUcode);	len += size;
	size = sizeof(mc_req_msg_r.len);	len += size;
	size = sizeof(mc_req_msg_r.timer);	len += size;
	size = sizeof(mc_req_msg_r.com);	len += size;
	size = sizeof(mc_req_msg_r.scom);	len += size;
	size = sizeof(mc_req_msg_r.d_no);	len += size;
	size = sizeof(mc_req_msg_r.d_no0);	len += size;
	size = sizeof(mc_req_msg_r.d_code);	len += size;
	size = sizeof(mc_req_msg_r.n_device); len += size;

	// len=21

	//	���M�o�b�t�@�̃T�[�o�[�A�h���X�ɑ��M 
	if (pMCSock->snd_msg((char*)read_req_snd_buf, len, pMCSock->addr_in_dst) == SOCKET_ERROR) {
		msg_wos.str() = pMCSock->err_msg.str();
		return S_FALSE;
	}
	return S_OK;
}

//*********************************************************************************************
/// <summary>
/// �\�P�b�g��M�C�x���g��M�f�[�^����
/// </summary>
/// <returns></returns>
UINT CMCProtocol::rcv_msg_3E(PINT16 pdst) {

	//�\�P�b�g��M����
	int nRtn = pMCSock->rcv_msg((char*)rcv_buf, sizeof(ST_XE_RES));
	if (nRtn == SOCKET_ERROR) {
		msg_wos.str() = pMCSock->err_msg.str();
		return S_FALSE;
	}
	
	//��M�f�[�^����
	ST_XE_RES res;
	UINT8* p8 = rcv_buf;	//rcv_buf=�N���X�̎�M�o�b�t�@�����o

	int size, len = 0;

	memcpy(&res.subcode, rcv_buf, size = sizeof(res.subcode));	len += size; p8 += size;
	if (res.subcode == CODE_4E_FORMAT) 	return S_FALSE;
	memcpy(&res.nNW, p8, size = sizeof(res.nNW));			len += size; p8 += size;
	memcpy(&res.nPC, p8, size = sizeof(res.nPC));			len += size; p8 += size;
	memcpy(&res.nUIO, p8, size = sizeof(res.nUIO));			len += size; p8 += size;
	memcpy(&res.nUcode, p8, size = sizeof(res.nUcode));		len += size; p8 += size;
	memcpy(&res.len, p8, size = sizeof(res.len));			len += size; p8 += size;
	memcpy(&res.endcode, p8, size = sizeof(res.endcode));	len += size; p8 += size;
	//�����܂łŃw�b�_�������[�J���o�b�t�@�ɓǂݍ���

	if (res.endcode) {	//�G���[�L
		memcpy(res.res_data, p8, size = res.len);	len += size;	//�f�[�^���ǂݍ��݁@res.len:�f�[�^��
		memcpy(&mc_res_msg_err, &res, len);
		return MC_RES_ERR;
	}
	else {
		if (res.len == 2) {//�������݉���
			memcpy(&mc_res_msg_w, &res, len);
			return MC_RES_WRITE;
		}
		else {
			memcpy(&mc_res_msg_r, &res, len);					//�N���X�̃��b�Z�[�W�o�b�t�@�Ƀw�b�_���܂ŃR�s�[
			memcpy(pdst, p8, size = res.len);	len += size;	//�f�[�^�����w��o�b�t�@�ɓǂݍ���
			return MC_RES_READ;
		}
	}

	return S_FALSE;

}

//*********************************************************************************************�@
/// <summary>
/// ���M�o�b�t�@�̓��e���\���̂ɓW�J
/// </summary>
/// <param name="p8"></param>
/// <param name="pbuf"></param>
/// <returns></returns>
HRESULT CMCProtocol::parse_snd_buf(UINT8* p8, LPST_XE_REQ pbuf) {
	int size, len = 0;
	memcpy(&pbuf->subcode, p8, size = sizeof(pbuf->subcode));	len += size; p8 += size;
	memcpy(&pbuf->nNW, p8, size = sizeof(pbuf->nNW));			len += size; p8 += size;
	memcpy(&pbuf->nPC, p8, size = sizeof(pbuf->nPC));			len += size; p8 += size;
	memcpy(&pbuf->nUIO, p8, size = sizeof(pbuf->nUIO));			len += size; p8 += size;
	memcpy(&pbuf->nUcode, p8, size = sizeof(pbuf->nUcode));		len += size; p8 += size;
	memcpy(&pbuf->len, p8, size = sizeof(pbuf->len));			len += size; p8 += size;
	memcpy(&pbuf->timer, p8, size = sizeof(pbuf->timer));		len += size; p8 += size;
	memcpy(&pbuf->com, p8, size = sizeof(pbuf->com));			len += size; p8 += size;
	memcpy(&pbuf->scom, p8, size = sizeof(pbuf->scom));			len += size; p8 += size;
	memcpy(&pbuf->d_no, p8, size = sizeof(pbuf->d_no));			len += size; p8 += size;//3�o�C�g�w�肾��2�o�C�g�Z�b�g0�`65535�͈̔�
	memcpy(&pbuf->d_no0, p8, size = sizeof(pbuf->d_no0));		len += size; p8 += size;//3�o�C�g�̎c��P�o�C�g�͖���
	memcpy(&pbuf->d_code, p8, size = sizeof(pbuf->d_code));		len += size; p8 += size;
	memcpy(&pbuf->n_device, p8, size = sizeof(pbuf->n_device)); len += size; p8 += size;

	if ((pbuf->n_device > 0) && (pbuf->n_device <= 300)) {
		for (int i = 0; i < pbuf->n_device; i++) pbuf->req_data[i] = *((INT16*)p8 + i);
	}
	else return S_FALSE;

	return S_OK;
}
//*********************************************************************************************�@
/// <summary>
/// �V�~�����[�^�p�FPLC�ւ̃R�}���h�։�����Ԃ��֐�
/// </summary>
/// <param name="pbuf">PLC��M�iPC���M�j�o�b�t�@</param>
/// <param name="pdata"> 
/// Read���FPLC���@PC�p�ǂݏo���p�f�o�C�X�͋[�������ւ̃|�C���^
/// Write���FPLC���@PC�������݃f�o�C�X�͋[�������ւ̃|�C���^
/// </param>
/// <returns></returns>
UINT16 CMCProtocol::snd_responce(ST_XE_REQ st_com, UINT16* pdata) {

	UINT8 buf_PLCsnd[MAX_MC_DATA * 2];
	UINT8* p8 = buf_PLCsnd;
	int len = 0;
	*p8 = 0xD0; p8++; len++;//�����T�u�R�[�h
	*p8 = 0x00; p8++; len++;//�����T�u�R�[�h
	*p8 = 0x00; p8++; len++;//�l�b�g���[�N�ԍ�
	*p8 = 0xFF; p8++; len++;//PC�ԍ�
	*p8 = 0xFF; p8++; len++;//���j�b�gIO�ԍ�
	*p8 = 0x03; p8++; len++;//���j�b�gIO�ԍ�
	*p8 = 0x00; p8++; len++;//���j�b�g�ǔԍ�

	if (st_com.com == CODE_COMMAND_READ) {
		*((UINT16*)p8) = 2 + st_com.n_device * 2; //�I���R�[�h���{�ǂݏo���f�o�C�X��
		p8 += 2; len += 2;//�����f�[�^��
		*p8 = 0x00; p8++; len++;//�I���R�[�h����
		*p8 = 0x00; p8++; len++;//�I���R�[�h����

		for (int i = 0; i < st_com.n_device; i++)
			((UINT16*)p8)[i] = pdata[i];

		len += st_com.n_device * 2;

		//	���M�o�b�t�@���N���C�A���g�A�h���X�ɑ��M 
		if (pMCSock->snd_msg((char*)buf_PLCsnd, len, pMCSock->addr_in_rcv) == SOCKET_ERROR) {
			msg_wos.str() = pMCSock->err_msg.str();
			return S_FALSE;
		}
		return st_com.com;
	}
	else if (st_com.com == CODE_COMMAND_WRITE) {
		*p8 = 0x02; p8++; len++;//�����f�[�^��
		*p8 = 0x00; p8++; len++;//�����f�[�^��
		*p8 = 0x00; p8++; len++;//�I���R�[�h����
		*p8 = 0x00; p8++; len++;//�I���R�[�h����

		for (int i = 0; i < st_com.n_device; i++)
			*(pdata + i) = st_com.req_data[i];

		//	���M�o�b�t�@���N���C�A���g�A�h���X�ɑ��M 
		if (pMCSock->snd_msg((char*)buf_PLCsnd, len, pMCSock->addr_in_rcv) == SOCKET_ERROR) {
			msg_wos.str() = pMCSock->err_msg.str();
			return S_FALSE;
		}

		return st_com.com;
	}
	else {
		return SOCKET_ERROR;
	}

	return st_com.com;
}

//*********************************************************************************************
SOCKADDR_IN CMCProtocol::get_addrin_rcv() {
	return pMCSock->addr_in_rcv;
}
SOCKADDR_IN CMCProtocol::get_addrin_snd() {
	return pMCSock->addr_in_dst;
}
SOCKADDR_IN CMCProtocol::get_addrin_from() {
	return pMCSock->addr_in_from;
}
