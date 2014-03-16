//==========================================
// StackTrace.cpp
//
// Win32 stacktracing mechanism, based on 
// article and code by Matt Pietrek 
// in MSDN Magazine, March 2002.
//
// Requires .pdb debug info to work properly.
//==========================================

// TODO: fix output to be double-click compatible with Output window in VS.

#include <sstream>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <dbghelp.h>
//#include <stdio.h>
//#include <tchar.h>

#include "StackTrace.hpp"

#pragma comment(linker, "/defaultlib:dbghelp.lib")

namespace util {

    class StackTrace
    {
    public:
        std::string WriteStackDetails( bool bWriteVariables );

    private:
        
        // Stolen from CVCONST.H in the DIA 2.0 SDK
        enum BasicType {
            btNoType = 0,
            btVoid = 1,
            btChar = 2,
            btWChar = 3,
            btInt = 6,
            btUInt = 7,
            btFloat = 8,
            btBCD = 9,
            btBool = 10,
            btLong = 13,
            btULong = 14,
            btCurrency = 25,
            btDate = 26,
            btVariant = 27,
            btComplex = 28,
            btBit = 29,
            btBSTR = 30,
            btHresult = 31
        };

        // Helper functions
        static StackTrace *inst;
        static BOOL CALLBACK sEnumerateSymbolsCallback(PSYMBOL_INFO,ULONG, PVOID);
        BOOL EnumerateSymbolsCallback(PSYMBOL_INFO,ULONG, PVOID);
        bool FormatSymbolValue( PSYMBOL_INFO, STACKFRAME *, char * pszBuffer, unsigned cbBuffer );
        char * DumpTypeIndex( char *, DWORD64, DWORD, unsigned, DWORD_PTR, bool & );
        char * FormatOutputValue( char * pszCurrBuffer, BasicType basicType, DWORD64 length, PVOID pAddress );
        BasicType GetBasicType( DWORD typeIndex, DWORD64 modBase );

        std::ostringstream ostr;
    };

    std::string getStackTrace(bool bWriteVariables)
    {
        // TODO: a mutex around this, since dbghelp is not threadsafe
        return StackTrace().WriteStackDetails(bWriteVariables);
    }

    StackTrace *StackTrace::inst;
    BOOL CALLBACK StackTrace::sEnumerateSymbolsCallback(PSYMBOL_INFO  pSymInfo, ULONG SymbolSize, PVOID UserContext)
    {
        return inst->EnumerateSymbolsCallback(pSymInfo, SymbolSize, UserContext);
    }

    std::string StackTrace::WriteStackDetails(bool bWriteVariables)
    {

        SymSetOptions( SYMOPT_DEFERRED_LOADS );
        if ( !SymInitialize( GetCurrentProcess(), 0, TRUE ) )
            return "";

        HANDLE hThread = GetCurrentThread();
        CONTEXT context;
        ::ZeroMemory( &context, sizeof(context) );
        context.ContextFlags = CONTEXT_FULL;
        GetThreadContext( hThread, &context );
        
        ostr.str("");
        ostr << "\nCall stack:\n";
        ostr << "Address   Frame     Function            SourceFile\n";

        DWORD dwMachineType = 0;
        // Could use SymSetOptions here to add the SYMOPT_DEFERRED_LOADS flag

        STACKFRAME sf;
        memset( &sf, 0, sizeof(sf) );

        #ifdef _M_IX86
        // Initialize the STACKFRAME structure for the first call.  This is only
        // necessary for Intel CPUs, and isn't mentioned in the documentation.
        sf.AddrPC.Offset       = context.Eip;
        sf.AddrStack.Offset    = context.Esp;
        sf.AddrFrame.Offset    = context.Ebp;
        sf.AddrPC.Mode         = AddrModeFlat;
        sf.AddrStack.Mode      = AddrModeFlat;
        sf.AddrFrame.Mode      = AddrModeFlat;
        dwMachineType = IMAGE_FILE_MACHINE_I386;
        #endif

        CONTEXT trashableContext = context;

        // JL
        // Need to call this once for some reason, for the subsequent calls to succeed.
        // TODO: why?
        StackWalk(  dwMachineType,
                        GetCurrentProcess(),
                        GetCurrentThread(),
                        &sf,
                        &trashableContext,
                        0,
                        SymFunctionTableAccess,
                        SymGetModuleBase,
                        0 );
        
        while ( 1 )
        {
            // Get the next stack frame
            if ( ! StackWalk(  dwMachineType,
                                GetCurrentProcess(),
                                GetCurrentThread(),
                                &sf,
                                &trashableContext,
                                0,
                                SymFunctionTableAccess,
                                SymGetModuleBase,
                                0 ) )
                break;

            if ( 0 == sf.AddrFrame.Offset ) // Basic sanity check to make sure
                break;                      // the frame is OK.  Bail if not.

            ostr << sf.AddrPC.Offset << "  " << sf.AddrFrame.Offset << "  " ;

            // Get the name of the function for this stack frame entry
            BYTE symbolBuffer[ sizeof(SYMBOL_INFO) + 1024 ];
            PSYMBOL_INFO pSymbol = (PSYMBOL_INFO)symbolBuffer;
            pSymbol->SizeOfStruct = sizeof(symbolBuffer);
            pSymbol->MaxNameLen = 1024;
                            
            DWORD64 symDisplacement = 0;    // Displacement of the input address,
                                            // relative to the start of the symbol

            if ( SymFromAddr(GetCurrentProcess(),sf.AddrPC.Offset,&symDisplacement,pSymbol))
            {
                ostr << pSymbol->Name << "+" << symDisplacement;
            }
            else    // No symbol found.  Print out the logical address instead.
            {
                //TCHAR szModule[MAX_PATH] = _T("");
                //DWORD section = 0, offset = 0;
                //GetLogicalAddress(  (PVOID)sf.AddrPC.Offset, szModule, sizeof(szModule), section, offset );
                //ostr << section << ":" << offset << " " << szModule ;
            }

            // Get the source line for this stack frame entry
            IMAGEHLP_LINE lineInfo = { sizeof(IMAGEHLP_LINE) };
            DWORD dwLineDisplacement;
            if ( SymGetLineFromAddr( GetCurrentProcess(), sf.AddrPC.Offset, &dwLineDisplacement, &lineInfo ) )
            {
                ostr << "  " << lineInfo.FileName << " line " << lineInfo.LineNumber;
            }

            ostr << "\n";

            // Write out the variables, if desired
            if ( bWriteVariables )
            {
                // Use SymSetContext to get just the locals/params for this frame
                IMAGEHLP_STACK_FRAME imagehlpStackFrame;
                imagehlpStackFrame.InstructionOffset = sf.AddrPC.Offset;
                SymSetContext( GetCurrentProcess(), &imagehlpStackFrame, 0 );

                // Enumerate the locals/parameters
                inst = this;
                SymEnumSymbols( GetCurrentProcess(), 0, 0, sEnumerateSymbolsCallback, &sf );

                ostr << "\n";
            }
        }

        SymCleanup( GetCurrentProcess() );

        return ostr.str();
    }

    BOOL StackTrace::EnumerateSymbolsCallback(PSYMBOL_INFO  pSymInfo,ULONG SymbolSize, PVOID UserContext)
    {
        char szBuffer[2048];
        if ( FormatSymbolValue( pSymInfo, (STACKFRAME*)UserContext, szBuffer, sizeof(szBuffer) ) )
        {
            ostr << "\t" << szBuffer << "\n";
        }
        return TRUE;
    }

    //////////////////////////////////////////////////////////////////////////////
    // Given a SYMBOL_INFO representing a particular variable, displays its
    // contents.  If it's a user defined type, display the members and their
    // values.
    //////////////////////////////////////////////////////////////////////////////
    bool StackTrace::FormatSymbolValue(
                PSYMBOL_INFO pSym,
                STACKFRAME * sf,
                char * pszBuffer,
                unsigned cbBuffer )
    {
        char * pszCurrBuffer = pszBuffer;

        // Indicate if the variable is a local or parameter
        if ( pSym->Flags & IMAGEHLP_SYMBOL_INFO_PARAMETER )
            pszCurrBuffer += sprintf( pszCurrBuffer, "Parameter " );
        else if ( pSym->Flags & IMAGEHLP_SYMBOL_INFO_LOCAL )
            pszCurrBuffer += sprintf( pszCurrBuffer, "Local " );

        // If it's a function, don't do anything.
        if ( pSym->Tag == 5 )   // SymTagFunction from CVCONST.H from the DIA SDK
            return false;

        // Emit the variable name
        pszCurrBuffer += sprintf( pszCurrBuffer, "\'%s\'", pSym->Name );

        DWORD_PTR pVariable = 0;    // Will point to the variable's data in memory

        if ( pSym->Flags & IMAGEHLP_SYMBOL_INFO_REGRELATIVE )
        {
            // if ( pSym->Register == 8 )   // EBP is the value 8 (in DBGHELP 5.1)
            {                               //  This may change!!!
                pVariable = sf->AddrFrame.Offset;
                pVariable += (DWORD_PTR)pSym->Address;
            }
            // else
            //  return false;
        }
        else if ( pSym->Flags & IMAGEHLP_SYMBOL_INFO_REGISTER )
        {
            return false;   // Don't try to report register variable
        }
        else
        {
            pVariable = (DWORD_PTR)pSym->Address;   // It must be a global variable
        }

        // Determine if the variable is a user defined type (UDT).  IF so, bHandled
        // will return true.
        bool bHandled;
        pszCurrBuffer = DumpTypeIndex(pszCurrBuffer,pSym->ModBase, pSym->TypeIndex,
                                        0, pVariable, bHandled );

        if ( !bHandled )
        {
            // The symbol wasn't a UDT, so do basic, stupid formatting of the
            // variable.  Based on the size, we're assuming it's a char, WORD, or
            // DWORD.
            BasicType basicType = GetBasicType( pSym->TypeIndex, pSym->ModBase );
            
            pszCurrBuffer = FormatOutputValue(pszCurrBuffer, basicType, pSym->Size,
                                                (PVOID)pVariable ); 
        }


        return true;
    }

    //////////////////////////////////////////////////////////////////////////////
    // If it's a user defined type (UDT), recurse through its members until we're
    // at fundamental types.  When he hit fundamental types, return
    // bHandled = false, so that FormatSymbolValue() will format them.
    //////////////////////////////////////////////////////////////////////////////
    char *StackTrace::DumpTypeIndex(
            char * pszCurrBuffer,
            DWORD64 modBase,
            DWORD dwTypeIndex,
            unsigned nestingLevel,
            DWORD_PTR offset,
            bool & bHandled )
    {
        bHandled = false;

        // Get the name of the symbol.  This will either be a Type name (if a UDT),
        // or the structure member name.
        WCHAR * pwszTypeName;
        if ( SymGetTypeInfo( GetCurrentProcess(), modBase, dwTypeIndex, TI_GET_SYMNAME,
                            &pwszTypeName ) )
        {
            pszCurrBuffer += sprintf( pszCurrBuffer, " %ls", pwszTypeName );
            LocalFree( pwszTypeName );
        }

        // Determine how many children this type has.
        DWORD dwChildrenCount = 0;
        SymGetTypeInfo( GetCurrentProcess(), modBase, dwTypeIndex, TI_GET_CHILDRENCOUNT,
                        &dwChildrenCount );

        if ( !dwChildrenCount )     // If no children, we're done
            return pszCurrBuffer;

        // Prepare to get an array of "TypeIds", representing each of the children.
        // SymGetTypeInfo(TI_FINDCHILDREN) expects more memory than just a
        // TI_FINDCHILDREN_PARAMS struct has.  Use derivation to accomplish this.
        struct FINDCHILDREN : TI_FINDCHILDREN_PARAMS
        {
            ULONG   MoreChildIds[1024];
            FINDCHILDREN(){Count = sizeof(MoreChildIds) / sizeof(MoreChildIds[0]);}
        } children;

        children.Count = dwChildrenCount;
        children.Start= 0;

        // Get the array of TypeIds, one for each child type
        if ( !SymGetTypeInfo( GetCurrentProcess(), modBase, dwTypeIndex, TI_FINDCHILDREN,
                                &children ) )
        {
            return pszCurrBuffer;
        }

        // Append a line feed
        pszCurrBuffer += sprintf( pszCurrBuffer, "\n" );

        // Iterate through each of the children
        for ( unsigned i = 0; i < dwChildrenCount; i++ )
        {
            // Add appropriate indentation level (since this routine is recursive)
            for ( unsigned j = 0; j <= nestingLevel+1; j++ )
                pszCurrBuffer += sprintf( pszCurrBuffer, "\t" );

            // Recurse for each of the child types
            bool bHandled2;
            pszCurrBuffer = DumpTypeIndex( pszCurrBuffer, modBase,
                                            children.ChildId[i], nestingLevel+1,
                                            offset, bHandled2 );

            // If the child wasn't a UDT, format it appropriately
            if ( !bHandled2 )
            {
                // Get the offset of the child member, relative to its parent
                DWORD dwMemberOffset;
                SymGetTypeInfo( GetCurrentProcess(), modBase, children.ChildId[i],
                                TI_GET_OFFSET, &dwMemberOffset );

                // Get the real "TypeId" of the child.  We need this for the
                // SymGetTypeInfo( TI_GET_TYPEID ) call below.
                DWORD typeId;
                SymGetTypeInfo( GetCurrentProcess(), modBase, children.ChildId[i],
                                TI_GET_TYPEID, &typeId );

                // Get the size of the child member
                ULONG64 length;
                SymGetTypeInfo(GetCurrentProcess(), modBase, typeId, TI_GET_LENGTH,&length);

                // Calculate the address of the member
                DWORD_PTR dwFinalOffset = offset + dwMemberOffset;

                BasicType basicType = GetBasicType(children.ChildId[i], modBase );

                pszCurrBuffer = FormatOutputValue( pszCurrBuffer, basicType,
                                                    length, (PVOID)dwFinalOffset ); 

                pszCurrBuffer += sprintf( pszCurrBuffer, "\n" );
            }
        }

        bHandled = true;
        return pszCurrBuffer;
    }

    char *StackTrace::FormatOutputValue(char * pszCurrBuffer, BasicType basicType, DWORD64 length, PVOID pAddress)
    {
        // Format appropriately (assuming it's a 1, 2, or 4 bytes (!!!)
        if ( length == 1 )
            pszCurrBuffer += sprintf( pszCurrBuffer, " = %X", *(PBYTE)pAddress );
        else if ( length == 2 )
            pszCurrBuffer += sprintf( pszCurrBuffer, " = %X", *(PWORD)pAddress );
        else if ( length == 4 )
        {
            if ( basicType == btFloat )
            {
                pszCurrBuffer += sprintf(pszCurrBuffer," = %f", *(PFLOAT)pAddress);
            }
            else if ( basicType == btChar )
            {
                if ( !IsBadStringPtr( *(PSTR*)pAddress, 32) )
                {
                    pszCurrBuffer += sprintf( pszCurrBuffer, " = \"%.31s\"",
                                                *(PDWORD)pAddress );
                }
                else
                    pszCurrBuffer += sprintf( pszCurrBuffer, " = %X",
                                                *(PDWORD)pAddress );
            }
            else
                pszCurrBuffer += sprintf(pszCurrBuffer," = %X", *(PDWORD)pAddress);
        }
        else if ( length == 8 )
        {
            if ( basicType == btFloat )
            {
                pszCurrBuffer += sprintf( pszCurrBuffer, " = %lf",
                                            *(double *)pAddress );
            }
            else
                pszCurrBuffer += sprintf( pszCurrBuffer, " = %I64X",
                                            *(DWORD64*)pAddress );
        }

        return pszCurrBuffer;
    }

    StackTrace::BasicType StackTrace::GetBasicType( DWORD typeIndex, DWORD64 modBase )
    {
        BasicType basicType;
        if ( SymGetTypeInfo( GetCurrentProcess(), modBase, typeIndex,
                            TI_GET_BASETYPE, &basicType ) )
        {
            return basicType;
        }

        // Get the real "TypeId" of the child.  We need this for the
        // SymGetTypeInfo( TI_GET_TYPEID ) call below.
        DWORD typeId;
        if (SymGetTypeInfo(GetCurrentProcess(),modBase, typeIndex, TI_GET_TYPEID, &typeId))
        {
            if ( SymGetTypeInfo( GetCurrentProcess(), modBase, typeId, TI_GET_BASETYPE,
                                &basicType ) )
            {
                return basicType;
            }
        }

        return btNoType;
    }

} // namespace util
