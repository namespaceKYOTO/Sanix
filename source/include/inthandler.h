#ifndef _INTHANDLER_H_
#define _INTHANDLER_H_

// 
void inthandler00( void );// ���Z�G���[
void inthandler01( void );// reserved. unused!
void inthandler02( void );// NMI���荞��
void inthandler03( void );// �u���[�N�|�C���g
void inthandler04( void );// �I�[�o�[�t���[
void inthandler05( void );// BOUND�͈̔͊O
void inthandler06( void );// �����I�y�R�[�h(����`�I�y�R�[�h)
void inthandler07( void );// �f�o�C�X�g�p�s��(�}�X�E�R�v���Z�b�T���Ȃ�)
void inthandler08( void );// �_�u���t�H���g
void inthandler09( void );// �R�v���Z�b�T�E�Z�O�����g�E�I�[�o�[����(�\��ς�)
void inthandler0a( void );// ����TSS
void inthandler0b( void );// �Z�O�����g�s��
void inthandler0c( void );// �X�^�b�N�E�Z�O�����g�E�t�H���g
void inthandler0d( void );// ��ʕی�
void inthandler0e( void );// �y�[�W�t�H���g
void inthandler0f( void );// Intel reserved. unused!
void inthandler10( void );// x87 FPU ���������_�G���[(�}�X�t�H���g)
void inthandler11( void );// �A���C�����g�E�`�F�b�N
void inthandler12( void );// �}�V���`�F�b�N
void inthandler13( void );// SIMD ���������_��O

// User defined
void inthandler20( void );// Timer
void inthandler21( void );// Keyboard

void inthandler2e( void );// ATA Master
void inthandler2f( void );// ATA Slave

// system call
void inthandler30( u32 idx );

void inthandler_ret( void );

#endif // _INTHANDLER_H_
