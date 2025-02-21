#include "pch.h"
#include "CSockLib.h"

#include <windows.h>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <TCHAR.H>

//****************************************************************************
/// <summary>
/// �R���X�g���N�^
/// </summary>
CSockBase::CSockBase()
{

}

//****************************************************************************
/// <summary>
/// �f�X�g���N�^
/// </summary>
/// <param name=""></param>
CSockBase::~CSockBase(void)
{

}
/******************************************************************************/
/// <summary>
/// WinSock�̏������@WSAStartup(MAKEWORD(1, 1))
/// </summary>
/// <returns>HRESULT</returns>
HRESULT CSockBase::Initialize()
{
	HRESULT	hr = S_OK;
	WSADATA	WsaData;
	if (WSAStartup(MAKEWORD(1, 1), &WsaData) == SOCKET_ERROR) {
		SetSockErr(WSAGetLastError());
		hr = E_FAIL;
	}	// ver1.01
	DeleteCriticalSection(&csSck);
	InitializeCriticalSection(&csSck);
	return hr;
}

/******************************************************************************/
/// <summary>
/// �N���X�����o�Ƀ\�P�b�g�̃G���[�R�[�h�ɉ������\����������Z�b�g����
/// </summary>
/// <param name="nError">GetLastError�Ŏ擾�����G���[�R�[�h</param>
/// <returns>HRESULT</returns>
HRESULT CSockBase::GetSockMsg(int nError)
{
	wchar_t	pszError[128];
	swprintf_s(pszError, sizeof(pszError) / 2, L"Err>> %d:", nError);					//�G���[�R�[�h��������

	switch (nError)
	{
		//lstrcat ���镶����̖����ɕʂ̕��������������B
	case WSAEINTR:				lstrcat(pszError, L"Interrupted system call");							break;
	case WSAEBADF:				lstrcat(pszError, L"Bad file number");									break;
	case WSAEACCES:				lstrcat(pszError, L"Permission denied");								break;
	case WSAEFAULT:				lstrcat(pszError, L"Bad address");										break;
	case WSAEINVAL:				lstrcat(pszError, L"Invalid argument");									break;
	case WSAEMFILE:				lstrcat(pszError, L"Too many open files");								break;
	case WSAEWOULDBLOCK:		lstrcat(pszError, L"Operation would block");							break;
	case WSAEINPROGRESS:		lstrcat(pszError, L"Operation now in progress");						break;
	case WSAEALREADY:			lstrcat(pszError, L"Operation already in progress");					break;
	case WSAENOTSOCK:			lstrcat(pszError, L"Socket operation on non-socket");					break;
	case WSAEDESTADDRREQ:		lstrcat(pszError, L"Destination address required");						break;
	case WSAEMSGSIZE:			lstrcat(pszError, L"Message too long");									break;
	case WSAEPROTOTYPE:			lstrcat(pszError, L"Protocol wrong type for socket");					break;
	case WSAENOPROTOOPT:		lstrcat(pszError, L"Protocol not available");							break;
	case WSAEPROTONOSUPPORT:	lstrcat(pszError, L"Protocol not supported");							break;
	case WSAESOCKTNOSUPPORT:	lstrcat(pszError, L"Socket type not supported");						break;
	case WSAEOPNOTSUPP:			lstrcat(pszError, L"Operation not supported on socket");				break;
	case WSAEPFNOSUPPORT:		lstrcat(pszError, L"Protocol family not supported");					break;
	case WSAEAFNOSUPPORT:		lstrcat(pszError, L"Address family not supported by protocol family");	break;
	case WSAEADDRINUSE:			lstrcat(pszError, L"Address already in use");							break;
	case WSAEADDRNOTAVAIL:		lstrcat(pszError, L"Can't assign requested address");					break;
	case WSAENETDOWN:			lstrcat(pszError, L"Network is down");									break;
	case WSAENETUNREACH:		lstrcat(pszError, L"Network is unreachable");							break;
	case WSAENETRESET:			lstrcat(pszError, L"Network dropped connection on reset");				break;
	case WSAECONNABORTED:		lstrcat(pszError, L"Software caused connection abort");					break;
	case WSAECONNRESET:			lstrcat(pszError, L"Connection reset by peer");							break;
	case WSAENOBUFS:			lstrcat(pszError, L"No buffer space available");						break;
	case WSAEISCONN:			lstrcat(pszError, L"Socket is already connected");						break;
	case WSAENOTCONN:			lstrcat(pszError, L"Socket is not connected");							break;
	case WSAESHUTDOWN:			lstrcat(pszError, L"Can't send after socket shutdown");					break;
	case WSAETOOMANYREFS:		lstrcat(pszError, L"Too many references: can't splice");				break;
	case WSAETIMEDOUT:			lstrcat(pszError, L"Connection timed out");								break;
	case WSAECONNREFUSED:		lstrcat(pszError, L"Connection refused");								break;
	case WSAELOOP:				lstrcat(pszError, L"Too many levels of symbolic links");				break;
	case WSAENAMETOOLONG:		lstrcat(pszError, L"File name too long");								break;
	case WSAEHOSTDOWN:			lstrcat(pszError, L"Host is down");										break;
	case WSAEHOSTUNREACH:		lstrcat(pszError, L"No route to host");									break;
	case WSAENOTEMPTY:			lstrcat(pszError, L"Directory not empty");								break;
	case WSAEPROCLIM:			lstrcat(pszError, L"Too many processes");								break;
	case WSAEUSERS:				lstrcat(pszError, L"Too many users");									break;
	case WSAEDQUOT:				lstrcat(pszError, L"Disc quota exceeded");								break;
	case WSAESTALE:				lstrcat(pszError, L"Stale NFS file handle");							break;
	case WSAEREMOTE:			lstrcat(pszError, L"Too many levels of remote in path");				break;
#ifdef _WIN32
	case WSAEDISCON:			lstrcat(pszError, L"Disconnect");										break;
#endif
	case WSASYSNOTREADY:		lstrcat(pszError, L"Network sub-system is unusable");					break;
	case WSAVERNOTSUPPORTED:	lstrcat(pszError, L"WinSock DLL cannot support this application");		break;
	case WSANOTINITIALISED:		lstrcat(pszError, L"WinSock not initialized");							break;
	case WSAHOST_NOT_FOUND:		lstrcat(pszError, L"Host not found");									break;
	case WSATRY_AGAIN:			lstrcat(pszError, L"Non-authoritative host not found");					break;
	case WSANO_RECOVERY:		lstrcat(pszError, L"Non-recoverable error");							break;
	case WSANO_DATA:			lstrcat(pszError, L"Valid name, no data record of requested type");		break;
		//	default:					lstrcpy(pszError, L"Not a WinSock error");								break;
	default:					lstrcat(pszError, L"Not a WinSock error");								break;
	}

	wstr_sock_err = pszError;

	return S_OK;
}

/******************************************************************************/
/// <summary>
/// 
/// </summary>
/// <param name="nError">GetLastError�Ŏ擾�����G���[�R�[�h</param>
/// <returns>HRESULT</returns>
HRESULT CSockBase::SetSockErr(int nError) {

	SYSTEMTIME st; TCHAR tbuf[32];
	::GetLocalTime(&st);
	wsprintf(tbuf, L"%02d:%02d:%02d.%01d", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds / 100);

	GetSockMsg(nError);
	err_msg.str(L"");
	err_msg << tbuf << L"::" << wstr_sock_err << L"@SockLib";

	//	err_msg << wstr_sock_err << L" <<" << _T(__FILE__) << L"@" << __LINE__ << L" >>";

	return S_OK;
}

/******************************************************************************/
/// <summary>
/// �\�P�b�g�N���[�Y
/// </summary>
/// <returns></returns>
HRESULT CSockBase::Close()
{
	closesocket(s);
	s = NULL;
	DeleteCriticalSection(&csSck);
	return (S_OK);
}




/******************************************************************************/
/// <summary>
/// �\�P�b�g�i��M�j�̏�����
/// </summary>
/// <param name="hwnd">�񓯊���M�C�x���g���󂯎��E�B���h�E�n���h��</param>
/// <param name="addr_in">�\�P�b�g�i��M�j�̃A�h���X/�|�[�g</param>
/// <returns></returns>
HRESULT CSockUDP::init_sock(HWND hwnd, SOCKADDR_IN addr_in) {

	//Socket�I�[�v��
	s = socket(AF_INET, protocol, 0);
	if (s < 0) {
		SetSockErr(WSAGetLastError());
		return E_FAIL;
	}

	//�\�P�b�g�ɖ��O��t���� bind
	addr_in_rcv = addr_in;
	int nRtn = bind(s, (LPSOCKADDR)&addr_in_rcv, (int)sizeof(addr_in_rcv));
	if (nRtn == SOCKET_ERROR) {
		SetSockErr(WSAGetLastError());
		closesocket(s);
		WSACleanup();
		return E_FAIL;
	}

	//�񓯊���
	nRtn = WSAAsyncSelect(s, hwnd, eventID, FD_READ | FD_WRITE | FD_CLOSE);
	if (nRtn == SOCKET_ERROR) {
		SetSockErr(WSAGetLastError());
		closesocket(s);
		WSACleanup();
		return E_FAIL;
	}

	return S_OK;
}
/******************************************************************************/
/// <summary>
/// �\�P�b�g�i��M�j�̏������@�}���`�L���X�g
/// </summary>
/// <param name="hwnd">�񓯊���M�C�x���g���󂯎��E�B���h�E�n���h��</param>
/// <param name="addr_in">�\�P�b�g�i��M�j�̃A�h���X/�|�[�g</param>
/// <returns></returns>
HRESULT CSockUDP::init_sock(HWND hwnd, SOCKADDR_IN addr_in, SOCKADDR_IN addr_in_m) {

	//Socket�I�[�v��
	s = socket(AF_INET, protocol, 0);
	if (s < 0) {
		SetSockErr(WSAGetLastError());
		return E_FAIL;
	}

	//�\�P�b�g�ɖ��O��t���� bind
	addr_in_rcv = addr_in;
	addr_in_multi[0] = addr_in_m;
	int nRtn = bind(s, (LPSOCKADDR)&addr_in_rcv, (int)sizeof(addr_in_rcv));
	if (nRtn == SOCKET_ERROR) {
		SetSockErr(WSAGetLastError());
		closesocket(s);
		WSACleanup();
		return E_FAIL;
	}

	//�񓯊���
	nRtn = WSAAsyncSelect(s, hwnd, eventID, FD_READ | FD_WRITE | FD_CLOSE);
	if (nRtn == SOCKET_ERROR) {
		SetSockErr(WSAGetLastError());
		closesocket(s);
		WSACleanup();
		return E_FAIL;
	}

	//TTL�ݒ�
	int ttl = 10;
	if (setsockopt(s, IPPROTO_IP, IP_MULTICAST_TTL, (char*)&ttl, sizeof(ttl) != 0)) {
		SetSockErr(WSAGetLastError());
		closesocket(s);
		WSACleanup();
		return E_FAIL;
	}

	addr_in_multi[0] = addr_in_m;						//�}���`�L���X�g���M��A�h���X�ݒ�
	static struct ip_mreq mreq;                     //�}���`�L���X�g��M�ݒ�p�\����
	//�}���`�L���X�g�O���[�v�Q���o�^
	memset(&mreq, 0, sizeof(mreq));
	mreq.imr_interface.S_un.S_addr = addr_in_rcv.sin_addr.S_un.S_addr;       //���p�l�b�g���[�N
	mreq.imr_multiaddr.S_un.S_addr = addr_in_multi[0].sin_addr.S_un.S_addr; 	//�}���`�L���X�gIP�A�h���X
	if (setsockopt(s, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mreq, sizeof(mreq)) != 0) {
		perror("setopt��M�ݒ莸�s\n");
		return E_FAIL;
	}
	n_multicast++;
	return S_OK;
}

/******************************************************************************/
/// <summary>
/// �\�P�b�g�i��M�j�̏������@�}���`�L���X�g
/// </summary>
/// <param name="hwnd">�񓯊���M�C�x���g���󂯎��E�B���h�E�n���h��</param>
/// <param name="addr_in">�\�P�b�g�i��M�j�̃A�h���X/�|�[�g</param>
/// <returns></returns>
HRESULT CSockUDP::add_multi_to_sock(SOCKADDR_IN addr_in_m) {

	if (n_multicast > MAX_N_MULTICAST) return E_FAIL;

	addr_in_multi[n_multicast] = addr_in_m;						//�}���`�L���X�g���M��A�h���X�ݒ�
	static struct ip_mreq mreq;                     //�}���`�L���X�g��M�ݒ�p�\����
	//�}���`�L���X�g�O���[�v�Q���o�^
	memset(&mreq, 0, sizeof(mreq));
	mreq.imr_interface.S_un.S_addr = addr_in_rcv.sin_addr.S_un.S_addr;       //���p�l�b�g���[�N
	mreq.imr_multiaddr.S_un.S_addr = addr_in_multi[n_multicast].sin_addr.S_un.S_addr; 	//�}���`�L���X�gIP�A�h���X
	if (setsockopt(s, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mreq, sizeof(mreq)) != 0) {
		perror("setopt��M�ݒ莸�s\n");
		return E_FAIL;
	}
	n_multicast++;
	return S_OK;
}
/******************************************************************************/
/// <summary>
/// ���b�Z�[�W���M
/// </summary>
/// <param name="pbuf">���M�f�[�^�o�b�t�@�|�C���^</param>
/// <param name="len">���M�f�[�^����</param>
/// <param name="to">���M��A�h���X�|�C���^</param>
/// <returns></returns>
int CSockUDP::snd_msg(const char* pbuf, int len, SOCKADDR_IN addr) {
	addr_in_dst = addr;
	int nRtn = sendto(s, pbuf, len, 0, (LPSOCKADDR)&addr_in_dst, sizeof(addr_in_dst));
	if (nRtn == SOCKET_ERROR) SetSockErr(WSAGetLastError());
	return nRtn;
}

/******************************************************************************/
/// <summary>
/// ���b�Z�[�W��M
/// </summary>
/// <param name="pbuf">��M�f�[�^�o�b�t�@�|�C���^</param>
/// <param name="len">��M�f�[�^�o�b�t�@�T�C�Y</param>
/// <returns></returns>
int CSockUDP::rcv_msg(char* pbuf, int len) {

	SOCKADDR from_addr;                             //���M���A�h���X��荞�݃o�b�t�@
	int from_addr_size = (int)sizeof(from_addr);    //���M���A�h���X�T�C�Y�o�b�t�@

	int nRtn = recvfrom(s, pbuf, len, 0, (SOCKADDR*)&addr_in_from, &from_addr_size);
	if (nRtn == SOCKET_ERROR) SetSockErr(WSAGetLastError());
	return nRtn;
}




//****************************************************************************
/// <summary>
/// �R���X�g���N�^
/// </summary>
/// <param name="_protocol">�@�v���g�R�� TCP/UDP�@</param>
/// <param name="_access">	�@�T�[�o�[/�N���C�A���g���</param>
CSockUDP::CSockUDP(INT32 _access, INT32 _eventID)
{
	addr_in_rcv.sin_port = htons(DEFAULT_MY_PORT);
	addr_in_rcv.sin_family = AF_INET;
	inet_pton(AF_INET, DEFAULT_MY_IP1, &addr_in_rcv.sin_addr.s_addr);
	addr_in_dst = addr_in_from = addr_in_rcv;
	for (int i = 0; i < MAX_N_MULTICAST; i++) addr_in_multi[i] = addr_in_rcv;

	eventID = _eventID;
	protocol = UDP_PROTOCOL;
	access = _access;		//��������\�P�b�g�̐ڑ��^�C�v

	InitializeCriticalSection(&csSck);
	s = -1;
}

/// <summary>
/// �R���X�g���N�^(�w�薳���Ƃ�	UDP	�N���C�A���g �C�x���gID�F10600
/// </summary>
CSockUDP::CSockUDP()
{
	addr_in_rcv.sin_port = htons(DEFAULT_MY_PORT);
	addr_in_rcv.sin_family = AF_INET;
	inet_pton(AF_INET, DEFAULT_MY_IP1, &addr_in_rcv.sin_addr.s_addr);
	addr_in_dst = addr_in_from = addr_in_rcv;
	for (int i = 0; i < MAX_N_MULTICAST; i++) addr_in_multi[i] = addr_in_rcv;

	eventID = DEFAULT_SOCK_EVENT;
	protocol = UDP_PROTOCOL;
	access = ACCESS_TYPE_CLIENT;		//��������\�P�b�g�̐ڑ��^�C�v
	n_multicast = 0;

	InitializeCriticalSection(&csSck);
	s = -1;
}

CSockUDP::~CSockUDP()
{
}



