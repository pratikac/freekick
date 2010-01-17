//==============================================================================
// $Id$
//==============================================================================

#include <windows.h>
#include <stdio.h>

unsigned int SerialTX(HANDLE hCom, char* buff, unsigned int count)
{
    BOOL bWriteRC;
    DWORD iBytesWritten=0;

    bWriteRC = WriteFile(hCom, buff, count, &iBytesWritten,NULL);

    return iBytesWritten;
}

void SerialPutC(HANDLE hCom, char txChar)
{
    BOOL bWriteRC;
    DWORD iBytesWritten;

    bWriteRC = WriteFile(hCom, &txChar, 1, &iBytesWritten, NULL);

    return;
}

char SerialGetC(HANDLE hCom)
{
    BOOL bReadRC;
    char c;
    DWORD iBytesRead;

    bReadRC = ReadFile(hCom, &c, 1, &iBytesRead, NULL);

    return c;
}

BOOL serialportInit(HANDLE *handle, const char *port, UINT32 baudRate)
{
    DCB dcb;
    HANDLE hCom;
    BOOL fSuccess;

    *handle = CreateFile( port,
            GENERIC_READ | GENERIC_WRITE,
            0,    // must be opened with exclusive-access
            NULL, // no security attributes
            OPEN_EXISTING, // must use OPEN_EXISTING
            0,    // not overlapped I/O
            NULL  // hTemplate must be NULL for comm devices
            );
    hCom = *handle;
    if (hCom == INVALID_HANDLE_VALUE) 
    {
        // Handle the error.
        printf ("CreateFile failed with error %d.\n", (int)GetLastError());
        return FALSE;
    }

    // Build on the current configuration, and skip setting the size
    // of the input and output buffers with SetupComm.

    fSuccess = GetCommState(hCom, &dcb);

    if (!fSuccess) 
    {
        // Handle the error.
        printf ("GetCommState failed with error %d.\n", (int)GetLastError());
        return FALSE;
    }

    // Fill in DCB: 115,200 bps, 8 data bits, no parity, and 1 stop bit.

    dcb.BaudRate = baudRate;     // set the baud rate
    dcb.ByteSize = 8;             // data size, xmit, and rcv
    dcb.Parity = NOPARITY;        // no parity bit
    dcb.StopBits = ONESTOPBIT;    // one stop bit

    fSuccess = SetCommState(hCom, &dcb);

    if (!fSuccess) 
    {
        // Handle the error.
        printf ("SetCommState failed with error %d.\n", (int)GetLastError());
        return FALSE;
    }

    printf ("Serial port %s successfully reconfigured.\n", port);

    return TRUE;
}
