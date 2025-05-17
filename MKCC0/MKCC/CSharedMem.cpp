#include "CSharedMem.h"
/****************************************************************************************************************************************************
�R���X�g���N�^�@�f�X�g���N�^
****************************************************************************************************************************************************/
CSharedMem::CSharedMem()
{
	smem_available = L_OFF;			//���L����������
	data_size = SMEM_DATA_SIZE_MAX;	//�f�[�^�T�C�Y
	hMapFile = NULL;
	pMapTop = smem_dummy_buf;
	dwExist = 0;
	hMutex = NULL;
}

CSharedMem::~CSharedMem()
{
	delete_smem(); 
}

/****************************************************************************************************************************************************
���L���������m�ۂ���
- ����
	-���́@LPCTSTR szName�F���L��������, DWORD dwSize�F�m�ۂ���T�C�Y,
	-�o�́@HANDLE* hMapFile�F�t�@�C���}�b�v�I�u�W�F�N�g�n���h��, LPVOID* pMapTop:���L�������擪�A�h���X, DWORD* dwExist:�@GetLastError()�G���[�L��

- �߂�l�@0:���튮���@����:�ُ튮��
****************************************************************************************************************************************************/
int CSharedMem::create_smem(LPCTSTR szName, DWORD dwSize, LPCTSTR szMuName)
{
	DWORD	highByte = 0;			// ���L��������32bit�T�C�Y�ȓ���z��
	DWORD	lowByte;

	lowByte = dwSize;		// 32bit�T�C�Y�ȏ�͒�`�s��
	data_size = dwSize;

	// ������
	hMapFile = NULL;
	pMapTop = NULL;
	dwExist = L_OFF;
	wstr_smem_name = szName;
	smem_available = L_OFF;

	// �t�@�C���E�}�b�s���O�E�I�u�W�F�N�g���쐬(�y�[�W�t�@�C�����g�p ,�Z�L�����e�B�����Ȃ� ,�ǂݍ���/�������݃A�N�Z�X ,���L�������̃T�C�Y ,���L��������)
	hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, highByte, lowByte, szName);
	if (hMapFile == NULL) {
		smem_available = L_OFF;
		return(ERR_SHMEM_CREATE);
	}
	else {
		if (GetLastError() == ERROR_ALREADY_EXISTS)	dwExist = L_ON;
		// �t�@�C���E�r���[���쐬(���L�������̃n���h��, �A�N�Z�X�̎��, �t�@�C�� �I�t�Z�b�g(���32�r�b�g), �t�@�C�� �I�t�Z�b�g(����32�r�b�g), �}�b�v����o�C�g����0�̓t�@�C��ك}�b�s���O�I�u�W�F�N�g�S��)
		pMapTop = MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		if (pMapTop == NULL) {
			CloseHandle(hMapFile);	// ���L�����������
			hMapFile = NULL; pMapTop = NULL;
			return(ERR_SHMEM_VIEW);
		}
		smem_available = L_ON;
	}

	clear_smem();

	hMutex= CreateMutex(NULL, FALSE, szMuName);
	if (hMutex == NULL) {
		smem_available = L_OFF;
		return ERR_SHMEM_MUTEX;
	}
	return(OK_SHMEM);
}

/****************************************************************************************************************************************************
���L���������J������
- �����@HANDLE* hMapFile,�F�����t�@�C���}�b�v�I�u�W�F�N�g LPVOID* pMapTop�F����惁�����擪�A�h���X
- �߂�l�@0:���튮��,-2:�ُ튮��
****************************************************************************************************************************************************/
int CSharedMem::delete_smem() {
	int		ret = OK_SHMEM;		// �֐��X�e�[�^�X

	CloseHandle(hMutex);

	if (pMapTop != NULL) {
		if (!UnmapViewOfFile(pMapTop)) {	// �t�@�C���E�r���[�̍폜
			ret = ERR_SHMEM_VIEW;
		}
	}

	// �t�@�C���E�}�b�s���O�E�I�u�W�F�N�g�����
	if (hMapFile != NULL) {
		CloseHandle(hMapFile);
	}

	// �|�C���^������
	hMapFile = NULL;
	pMapTop = NULL;

	return(ret);

}
/****************************************************************************************************************************************************
���L��������0�N���A����
- �����@�Ȃ�
- �߂�l�@L_ON:���튮��,�@L_OFF:�ُ튮��
****************************************************************************************************************************************************/
int CSharedMem::clear_smem() {

	if (!smem_available)
		return ERR_SHMEM_NOT_AVAILABLE;
	
	memset(pMapTop, 0, data_size);
	
	return(OK_SHMEM);
}

