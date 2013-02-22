// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__8692AECA_5BC3_4136_9B64_74A4DC55AE75__INCLUDED_)
#define AFX_STDAFX_H__8692AECA_5BC3_4136_9B64_74A4DC55AE75__INCLUDED_

#ifndef __GNUC__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if _MSC_VER > 1200
#include "..\\common\\targetver.h"
#endif

#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_DEPRECATE
#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afx.h>
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <iostream>

// TODO: reference additional headers your program requires here

#include "..\\..\\include\\ar168.h"

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#else // __GNUC__
#include <sys/stat.h>
#include "../common/mfc2std.h"
#include "../common/common.h"
#include "../../include/version.h"
#include "../../include/ar168.h"
#endif

#endif // !defined(AFX_STDAFX_H__8692AECA_5BC3_4136_9B64_74A4DC55AE75__INCLUDED_)
