
// ��ȣȭ ���
#define ENC 1 
// ��ȣȭ ���
#define DEC 0 

typedef unsigned char BYTE;

// 128��Ʈ AES �Ϻ�ȣȭ �������̽�
void AES128(BYTE* plain, BYTE* cipher, BYTE* key, int mode);