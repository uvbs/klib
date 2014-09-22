#include "utility.h"
#include <windows.h>
#include <tchar.h>

wstring GetFileName( const wstring& fullname )
{
	wstring ret;
	int pos = fullname.rfind( '\\' );
	
	if ( 0 < pos && fullname.size()-1 > pos )
		ret = fullname.substr( pos+1 );
	else
		ret = fullname;

	return ret;
}


/* Returns TRUE if the key is allowed and its name is resolved. */
bool GetKeyName ( unsigned int vKey, wchar_t *keyName)
{
	bool result = true;

	if (vKey >= 0x30 && vKey <= 0x5a)	
	{
		// ASCII characters
		wsprintfW (keyName, L"%hc", (char) vKey);
	}
	else if (vKey >= 0xE9 && vKey <= 0xF5)	
	{
		// OEM-specific
		wsprintfW (keyName, L"OEM-%d", vKey);
	}
	else if (vKey >= VK_F1 && vKey <= VK_F24)
	{
		// F1-F24
		wsprintfW (keyName, L"F%d", vKey - VK_F1 + 1);
	}
	else if (vKey >= VK_NUMPAD0 && vKey <= VK_NUMPAD9)
	{
		// Numpad numbers
		wsprintfW (keyName, L"%s %d", _T("VK_NUMPAD"), vKey - VK_NUMPAD0); 
	}
	else
	{
		switch (vKey)
		{
		case VK_MULTIPLY:	wsprintfW (keyName, L"%s *", _T("VK_NUMPAD")); break;
		case VK_ADD:		wsprintfW (keyName, L"%s +", _T("VK_NUMPAD")); break;
		case VK_SEPARATOR:	wsprintfW (keyName, L"%s Separator", _T("VK_NUMPAD")); break;
		case VK_SUBTRACT:	wsprintfW (keyName, L"%s -", _T("VK_NUMPAD")); break;
		case VK_DECIMAL:	wsprintfW (keyName, L"%s .", _T("VK_NUMPAD")); break;
		case VK_DIVIDE:		wsprintfW (keyName, L"%s /", _T("VK_NUMPAD")); break;
		case VK_OEM_1:		wcscpy (keyName, L"OEM 1 (';')"); break;
		case VK_OEM_PLUS:	wcscpy (keyName, L"+"); break;
		case VK_OEM_COMMA:	wcscpy (keyName, L","); break;
		case VK_OEM_MINUS:	wcscpy (keyName, L"-"); break;
		case VK_OEM_PERIOD:	wcscpy (keyName, L".");	break;
		case VK_OEM_2:		wcscpy (keyName, L"OEM 2 ('/')"); break;
		case VK_OEM_3:		wcscpy (keyName, L"OEM 3 (`)"); break;
		case VK_OEM_4:		wcscpy (keyName, L"OEM 4 ('[')"); break;
		case VK_OEM_5:		wcscpy (keyName, L"OEM 5 ('\\')"); break;
		case VK_OEM_6:		wcscpy (keyName, L"OEM 6 (']')"); break;
		case VK_OEM_7:		wcscpy (keyName, L"OEM 7 (')"); break;
		case VK_OEM_8:		wcscpy (keyName, L"OEM 8"); break;
		case VK_OEM_AX:		wcscpy (keyName, L"OEM AX"); break;
		case VK_OEM_102:	wcscpy (keyName, L"OEM 102"); break;
		case VK_ICO_HELP:	wcscpy (keyName, L"ICO_HELP"); break;
		case VK_ICO_00:		wcscpy (keyName, L"ICO_00"); break;
		case VK_ICO_CLEAR:	wcscpy (keyName, L"ICO_CLEAR"); break;
		case VK_ATTN:		wcscpy (keyName, L"Attn"); break;
		case VK_CRSEL:		wcscpy (keyName, L"CrSel"); break;
		case VK_EXSEL:		wcscpy (keyName, L"ExSel"); break;
		case VK_EREOF:		wcscpy (keyName, L"Erase EOF"); break;
		case VK_PA1:		wcscpy (keyName, L"PA1"); break;
		case VK_OEM_CLEAR:	wcscpy (keyName, L"OEM Clear"); break;

		case 0:
		case 1:
		case 0xFF:
			result = false;
			break;

		default:
			swprintf (keyName, L"VKEY_%02X", vKey);
		}
	}
	return result;
}
