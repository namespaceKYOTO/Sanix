#ifndef _INTHANDLER_H_
#define _INTHANDLER_H_

// 
void inthandler00( void );// 除算エラー
void inthandler01( void );// reserved. unused!
void inthandler02( void );// NMI割り込み
void inthandler03( void );// ブレークポイント
void inthandler04( void );// オーバーフロー
void inthandler05( void );// BOUNDの範囲外
void inthandler06( void );// 無効オペコード(未定義オペコード)
void inthandler07( void );// デバイス使用不可(マス・コプロセッサがない)
void inthandler08( void );// ダブルフォルト
void inthandler09( void );// コプロセッサ・セグメント・オーバーラン(予約済み)
void inthandler0a( void );// 無効TSS
void inthandler0b( void );// セグメント不在
void inthandler0c( void );// スタック・セグメント・フォルト
void inthandler0d( void );// 一般保護
void inthandler0e( void );// ページフォルト
void inthandler0f( void );// Intel reserved. unused!
void inthandler10( void );// x87 FPU 浮動小数点エラー(マスフォルト)
void inthandler11( void );// アライメント・チェック
void inthandler12( void );// マシンチェック
void inthandler13( void );// SIMD 浮動小数点例外

// User defined
void inthandler20( void );// Timer
void inthandler21( void );// Keyboard

void inthandler2e( void );// ATA Master
void inthandler2f( void );// ATA Slave

// system call
void inthandler30( u32 idx );

void inthandler_ret( void );

#endif // _INTHANDLER_H_
