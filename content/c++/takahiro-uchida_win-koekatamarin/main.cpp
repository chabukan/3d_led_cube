#include "ShowText.h"

int main(int argc, const char* argv[])
{
	Initialize("ledLib32.dll", argc, argv);
	for (;;) {
		CString text(_T("�I���̖��O�͓��c���I"));
		ShowText(text);
		exit(0);
    }
}
