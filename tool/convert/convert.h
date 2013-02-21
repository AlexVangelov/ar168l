
#if !defined(AFX_CONVERT_H__7DB030E0_56E2_4252_B8F0_490BB0D1D828__INCLUDED_)
#define AFX_CONVERT_H__7DB030E0_56E2_4252_B8F0_490BB0D1D828__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../include/ar168.h"

#define DATA_RING_SIZE			(SYSTEM_RINGTONE_PAGE_NUM * FILE_FULL_PAGE_SIZE)
//#define CFG_FILE_SIZE			65536
#define FRAME_LEN				16
#define DATA_BLOCK_LEN			32640 //(136*240)

#define IVR_PAGE_SIZE		0x7ee0
#define IVR_FILE_SIZE		FILE_FULL_PAGE_SIZE

// in functions.cpp
void OnBin2Cfg(CString strSrcFile, CString strDstFile, BOOL bForce, BOOL bInnomedia);
void OnCfg2Bin(CString strSrcFile, CString strDstFile, BOOL bForce);
void OnBin2H(CString strSrcFile, CString strDstFile, BOOL bForce, int iAddressOffset, int iConvertLength);
void OnRing2Bin(CString strSrcFile, CString strDstFile, BOOL bForce);
void OnIvr2Bin(CString strSrcFile, CString strDstFile, BOOL bForce);
void OnPB2Bin(CString strSrcFile, CString strDstFile, BOOL bForce);
void OnBin2PB(CString strSrcFile, CString strDstFile, BOOL bForce);
void OnDsp2H(CStringList& strFileList, CString strSrcDir, CString strDstDir, BOOL bForce);

// in file web.cpp
void OnWeb2Web(CString strSrcFile, CString strDstFile, BOOL bForce);
void OnWeb2Info(CString strSrcFile, CString strDstFile, BOOL bForce);

#endif // !defined(AFX_CONVERT_H__7DB030E0_56E2_4252_B8F0_490BB0D1D828__INCLUDED_)
