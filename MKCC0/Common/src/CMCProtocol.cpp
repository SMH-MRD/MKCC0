#include <stdexcept>
#include "CMCProtocol.h"
#include "CSockLib.h"

static CSockUDP* pMCSock;

//グローバル
LARGE_INTEGER frequency;                //システムの周波数
LARGE_INTEGER start_count_w, end_count_w, start_count_r, end_count_r;   //システムカウント数
LONGLONG spancount_max_w, spancount_max_r;

//*********************************************************************************************　
/// <summary>
/// コンストラクタ
/// </summary>
CMCProtocol::CMCProtocol(INT32 _eventID) {
	eventID = _eventID;
	D_no_r = D_no_w = n_D_read = n_D_write = 0;
	memset(&rcv_buf, 0, sizeof(rcv_buf));
	memset(&snd_buf, 0, sizeof(snd_buf));

}
//*********************************************************************************************　
/// <summary>
/// デストラクタ
/// </summary>
CMCProtocol::~CMCProtocol() {
	delete pMCSock;
	close();
}
//*********************************************************************************************　
/// <summary>
/// 指定ソケット準備、読み書きデバイス指定
/// </summary>
/// <param name="hWnd">ソケットイベントを受信するウィンドウのハンドル</param>
/// <param name="paddrinc">	受信ソケットアドレス</param>
/// <param name="paddrins">	送信ソケットアドレス</param>
/// <param name="no_r">		読み出しDデバイス先頭番号</param>
/// <param name="num_r">	読み出しDデバイス数</param>
/// <param name="no_w">		書き込みDデバイス先頭番号</param>
/// <param name="num_w">	書き込みDデバイス数号</param>
/// <returns></returns>
HRESULT CMCProtocol::Initialize(HWND hwnd, int type) {
	switch (type) {
	case PLC_IF_TYPE_SLBRK: //SLBRK IF
	{
		//読み書きデバイス設定
		set_access_D_r(SLBRK_MC_ADDR_W_READ, SLBRK_MC_SIZE_W_READ);  //読み出しDデバイス先頭アドレスセット
		set_access_D_w(SLBRK_MC_ADDR_W_WRITE, SLBRK_MC_SIZE_W_WRITE);//書き込みDデバイス先頭アドレスセット

		//送信バッファフォーマット（ヘッダ部）設定
		set_sndbuf_read_D_3E();		//3E　Dデバイス読み込み用要求送信フォーマットセット
		set_sndbuf_write_D_3E();	//3E　Dデバイス書き込み用要求送信フォーマットセット

		//クライアントUDPソケット　アドレス設定　インスタンス化　初期化

		pMCSock = new CSockUDP(ACCESS_TYPE_CLIENT, eventID);
		if (pMCSock->Initialize() != S_OK) {							//受信ソケット生成
			msg_wos.str() = pMCSock->err_msg.str();
			return S_FALSE;
		}
		else {
			pMCSock->set_sock_addr(&pMCSock->addr_in_rcv, IP_ADDR_MC_CLIENT_SLBRK, PORT_MC_CLIENT_SLBRK);
			if (pMCSock->init_sock(hwnd, pMCSock->addr_in_rcv) != S_OK) {//受信ソケット設定
				msg_wos.str() = pMCSock->err_msg.str();
				return S_FALSE;
			}
		}

		//送信先アドレスセット
		pMCSock->set_sock_addr(&pMCSock->addr_in_dst, IP_ADDR_MC_SERVER_SLBRK, PORT_MC_SERVER_SLBRK);
	}break;
	case PLC_IF_TYPE_OTE: //OTE IF
	{
		//読み書きデバイス設定
		set_access_D_r(OTE_MC_ADDR_W_READ, OTE_MC_SIZE_W_READ);  //読み出しDデバイス先頭アドレスセット
		set_access_D_w(OTE_MC_ADDR_W_WRITE, OTE_MC_SIZE_W_WRITE);//書き込みDデバイス先頭アドレスセット

		//送信バッファフォーマット（ヘッダ部）設定
		set_sndbuf_read_D_3E();		//3E　Dデバイス読み込み用要求送信フォーマットセット
		set_sndbuf_write_D_3E();	//3E　Dデバイス書き込み用要求送信フォーマットセット

		//クライアントUDPソケット　アドレス設定　インスタンス化　初期化

		pMCSock = new CSockUDP(ACCESS_TYPE_CLIENT, eventID);
		if (pMCSock->Initialize() != S_OK) {							//受信ソケット生成
			msg_wos.str() = pMCSock->err_msg.str();
			return S_FALSE;
		}
		else {
			pMCSock->set_sock_addr(&pMCSock->addr_in_rcv, IP_ADDR_MC_CLIENT_OTE, PORT_MC_CLIENT_OTE);
			if (pMCSock->init_sock(hwnd, pMCSock->addr_in_rcv) != S_OK) {//受信ソケット設定
				msg_wos.str() = pMCSock->err_msg.str();
				return S_FALSE;
			}
		}

		//送信先アドレスセット
		pMCSock->set_sock_addr(&pMCSock->addr_in_dst, IP_ADDR_MC_SERVER_OTE, PORT_MC_SERVER_OTE);
	}break;
	case PLC_IF_TYPE_CC: //CC IF
	default:
	{
		//読み書きデバイス設定
		set_access_D_r(CC_MC_ADDR_W_READ, CC_MC_SIZE_W_READ);  //読み出しDデバイス先頭アドレスセット
		set_access_D_w(CC_MC_ADDR_W_WRITE, CC_MC_SIZE_W_WRITE);//書き込みDデバイス先頭アドレスセット

		//送信バッファフォーマット（ヘッダ部）設定
		set_sndbuf_read_D_3E();		//3E　Dデバイス読み込み用要求送信フォーマットセット
		set_sndbuf_write_D_3E();	//3E　Dデバイス書き込み用要求送信フォーマットセット

		//クライアントUDPソケット　アドレス設定　インスタンス化　初期化

		pMCSock = new CSockUDP(ACCESS_TYPE_CLIENT, eventID);
		if (pMCSock->Initialize() != S_OK) {									//受信ソケット生成
			msg_wos.str() = pMCSock->err_msg.str();
			return S_FALSE;
		}
		else {
			pMCSock->set_sock_addr(&pMCSock->addr_in_rcv, IP_ADDR_MC_CLIENT, PORT_MC_CLIENT);
			if (pMCSock->init_sock(hwnd, pMCSock->addr_in_rcv) != S_OK) {//受信ソケット設定
				msg_wos.str() = pMCSock->err_msg.str();
				return S_FALSE;
			}
		}

		//送信先アドレスセット
		pMCSock->set_sock_addr(&pMCSock->addr_in_dst, IP_ADDR_MC_SERVER, PORT_MC_SERVER);
	}
	}
	return S_OK;
}
//**********************************************************************************************　
/// <summary>
/// クローズ処理
/// </summary>
/// <returns></returns>
HRESULT CMCProtocol::close() {

	delete pMCSock;

	return 0;
}
//**********************************************************************************************　
/// <summary>
/// 3Eフォーマット Wデバイス読み込み用送信バッファセット
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

	mc_req_msg_r.len = 12; 	//以下のバイト数
	memcpy(p8, &mc_req_msg_r.len, size = sizeof(mc_req_msg_r.len));			len += size; p8 += size;

	mc_req_msg_r.timer = CODE_MON_TIMER;
	memcpy(p8, &mc_req_msg_r.timer, size = sizeof(mc_req_msg_r.timer));		len += size; p8 += size;

	mc_req_msg_r.com = CODE_COMMAND_READ;
	memcpy(p8, &mc_req_msg_r.com, size = sizeof(mc_req_msg_r.com));			len += size; p8 += size;

	mc_req_msg_r.scom = CODE_SUBCOM_READ;
	memcpy(p8, &mc_req_msg_r.scom, size = sizeof(mc_req_msg_r.scom));		len += size; p8 += size;

	mc_req_msg_r.d_no = D_no_r;
	memcpy(p8, &mc_req_msg_r.d_no, size = sizeof(mc_req_msg_r.d_no));		len += size; p8 += size;

	mc_req_msg_r.d_no0 = CODE_3E_NULL;	//簡単のため上位１byteは使わない
	memcpy(p8, &mc_req_msg_r.d_no0, size = sizeof(mc_req_msg_r.d_no0));		len += size; p8 += size;

	mc_req_msg_r.d_code = CODE_DEVICE_D;
	memcpy(p8, &mc_req_msg_r.d_code, size = sizeof(mc_req_msg_r.d_code));	len += size; p8 += size;

	mc_req_msg_r.n_device = n_D_read;
	memcpy(p8, &mc_req_msg_r.n_device, size = sizeof(mc_req_msg_r.n_device)); len += size; p8 += size;

	return S_OK;
}
//**********************************************************************************************　
/// <summary>
/// //3Eフォーマット Wデバイス書き込み用送信バッファヘッダ部セット
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


	mc_req_msg_w.len = 12 + n_D_write * 2; 	//以下のバイト数
	memcpy(p8, &mc_req_msg_w.len, size = sizeof(mc_req_msg_w.len));			len += size; p8 += size;

	mc_req_msg_w.timer = CODE_MON_TIMER;
	memcpy(p8, &mc_req_msg_w.timer, size = sizeof(mc_req_msg_w.timer));		len += size; p8 += size;

	mc_req_msg_w.com = CODE_COMMAND_WRITE;
	memcpy(p8, &mc_req_msg_w.com, size = sizeof(mc_req_msg_w.com));			len += size; p8 += size;

	mc_req_msg_w.scom = CODE_SUBCOM_WRITE;
	memcpy(p8, &mc_req_msg_w.scom, size = sizeof(mc_req_msg_w.scom));		len += size; p8 += size;

	mc_req_msg_w.d_no = D_no_w;
	memcpy(p8, &mc_req_msg_w.d_no, size = sizeof(mc_req_msg_w.d_no));		len += size; p8 += size;

	mc_req_msg_w.d_no0 = CODE_3E_NULL;//簡単のため上位１byteは使わない
	memcpy(p8, &mc_req_msg_w.d_no0, size = sizeof(mc_req_msg_w.d_no0));		len += size; p8 += size;

	mc_req_msg_w.d_code = CODE_DEVICE_D;
	memcpy(p8, &mc_req_msg_w.d_code, size = sizeof(mc_req_msg_w.d_code));	len += size; p8 += size;

	mc_req_msg_w.n_device = n_D_write;
	memcpy(p8, &mc_req_msg_w.n_device, size = sizeof(mc_req_msg_w.n_device)); len += size; p8 += size;

	for (int i = 0; i < n_D_write; i++) *(p8 + i) = 0;

	return S_OK;
}

//**********************************************************************************************　
/// <summary>
///	3Eフォーマット Wデバイス書き込み要求送信
/// </summary>
/// <param name="p_data"></param>
/// <param name="n_write"></param>
/// <returns></returns>
HRESULT CMCProtocol::send_write_req_D_3E(void* p_data) {

	UINT8* p8 = write_req_snd_buf;
	int len = 0, size = 0;
	//ヘッダ部分の書き込み無し
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
	//データ部のデータ更新
	memcpy(p8 + len, p_data, size = n_D_write * 2); len += size;

	//	送信バッファのサーバーアドレスに送信 
	if (pMCSock->snd_msg((char*)write_req_snd_buf, len, pMCSock->addr_in_dst) == SOCKET_ERROR) {
		msg_wos.str() = pMCSock->err_msg.str();
		return S_FALSE;
	}
	return S_OK;
}
//**********************************************************************************************　
/// <summary>
/// 3Eフォーマット Wデバイス読み込み要求送信
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

	//	送信バッファのサーバーアドレスに送信 
	if (pMCSock->snd_msg((char*)read_req_snd_buf, len, pMCSock->addr_in_dst) == SOCKET_ERROR) {
		msg_wos.str() = pMCSock->err_msg.str();
		return S_FALSE;
	}
	return S_OK;
}

//*********************************************************************************************
/// <summary>
/// ソケット受信イベント受信データ分析
/// </summary>
/// <returns></returns>
UINT CMCProtocol::rcv_msg_3E(PINT16 pdst) {

	//ソケット受信処理
	int nRtn = pMCSock->rcv_msg((char*)rcv_buf, sizeof(ST_XE_RES));
	if (nRtn == SOCKET_ERROR) {
		msg_wos.str() = pMCSock->err_msg.str();
		return S_FALSE;
	}
	
	//受信データ分析
	ST_XE_RES res;
	UINT8* p8 = rcv_buf;	//rcv_buf=クラスの受信バッファメンバ

	int size, len = 0;

	memcpy(&res.subcode, rcv_buf, size = sizeof(res.subcode));	len += size; p8 += size;
	if (res.subcode == CODE_4E_FORMAT) 	return S_FALSE;
	memcpy(&res.nNW, p8, size = sizeof(res.nNW));			len += size; p8 += size;
	memcpy(&res.nPC, p8, size = sizeof(res.nPC));			len += size; p8 += size;
	memcpy(&res.nUIO, p8, size = sizeof(res.nUIO));			len += size; p8 += size;
	memcpy(&res.nUcode, p8, size = sizeof(res.nUcode));		len += size; p8 += size;
	memcpy(&res.len, p8, size = sizeof(res.len));			len += size; p8 += size;
	memcpy(&res.endcode, p8, size = sizeof(res.endcode));	len += size; p8 += size;
	//ここまででヘッダ部をローカルバッファに読み込み

	if (res.endcode) {	//エラー有
		memcpy(res.res_data, p8, size = res.len);	len += size;	//データ部読み込み　res.len:データ長
		memcpy(&mc_res_msg_err, &res, len);
		return MC_RES_ERR;
	}
	else {
		if (res.len == 2) {//書き込み応答
			memcpy(&mc_res_msg_w, &res, len);
			return MC_RES_WRITE;
		}
		else {
			memcpy(&mc_res_msg_r, &res, len);					//クラスのメッセージバッファにヘッダ部までコピー
			memcpy(pdst, p8, size = res.len);	len += size;	//データ部を指定バッファに読み込み
			return MC_RES_READ;
		}
	}

	return S_FALSE;

}

//*********************************************************************************************　
/// <summary>
/// 送信バッファの内容を構造体に展開
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
	memcpy(&pbuf->d_no, p8, size = sizeof(pbuf->d_no));			len += size; p8 += size;//3バイト指定だが2バイトセット0～65535の範囲
	memcpy(&pbuf->d_no0, p8, size = sizeof(pbuf->d_no0));		len += size; p8 += size;//3バイトの残り１バイトは無視
	memcpy(&pbuf->d_code, p8, size = sizeof(pbuf->d_code));		len += size; p8 += size;
	memcpy(&pbuf->n_device, p8, size = sizeof(pbuf->n_device)); len += size; p8 += size;

	if ((pbuf->n_device > 0) && (pbuf->n_device <= 300)) {
		for (int i = 0; i < pbuf->n_device; i++) pbuf->req_data[i] = *((INT16*)p8 + i);
	}
	else return S_FALSE;

	return S_OK;
}
//*********************************************************************************************　
/// <summary>
/// シミュレータ用：PLCへのコマンドへ応答を返す関数
/// </summary>
/// <param name="pbuf">PLC受信（PC送信）バッファ</param>
/// <param name="pdata"> 
/// Read時：PLC内　PC用読み出し用デバイス模擬メモリへのポインタ
/// Write時：PLC内　PC書き込みデバイス模擬メモリへのポインタ
/// </param>
/// <returns></returns>
UINT16 CMCProtocol::snd_responce(ST_XE_REQ st_com, UINT16* pdata) {

	UINT8 buf_PLCsnd[MAX_MC_DATA * 2];
	UINT8* p8 = buf_PLCsnd;
	int len = 0;
	*p8 = 0xD0; p8++; len++;//応答サブコード
	*p8 = 0x00; p8++; len++;//応答サブコード
	*p8 = 0x00; p8++; len++;//ネットワーク番号
	*p8 = 0xFF; p8++; len++;//PC番号
	*p8 = 0xFF; p8++; len++;//ユニットIO番号
	*p8 = 0x03; p8++; len++;//ユニットIO番号
	*p8 = 0x00; p8++; len++;//ユニット局番号

	if (st_com.com == CODE_COMMAND_READ) {
		*((UINT16*)p8) = 2 + st_com.n_device * 2; //終了コード長＋読み出しデバイス長
		p8 += 2; len += 2;//応答データ長
		*p8 = 0x00; p8++; len++;//終了コード正常
		*p8 = 0x00; p8++; len++;//終了コード正常

		for (int i = 0; i < st_com.n_device; i++)
			((UINT16*)p8)[i] = pdata[i];

		len += st_com.n_device * 2;

		//	送信バッファをクライアントアドレスに送信 
		if (pMCSock->snd_msg((char*)buf_PLCsnd, len, pMCSock->addr_in_rcv) == SOCKET_ERROR) {
			msg_wos.str() = pMCSock->err_msg.str();
			return S_FALSE;
		}
		return st_com.com;
	}
	else if (st_com.com == CODE_COMMAND_WRITE) {
		*p8 = 0x02; p8++; len++;//応答データ長
		*p8 = 0x00; p8++; len++;//応答データ長
		*p8 = 0x00; p8++; len++;//終了コード正常
		*p8 = 0x00; p8++; len++;//終了コード正常

		for (int i = 0; i < st_com.n_device; i++)
			*(pdata + i) = st_com.req_data[i];

		//	送信バッファをクライアントアドレスに送信 
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
