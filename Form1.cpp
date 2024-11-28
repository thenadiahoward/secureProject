#include "Form1.h"
//version 3
using namespace System;
using namespace System::Windows::Forms;

[STAThread]
int main(array<String^>^ args)
{
    Application::EnableVisualStyles();
    Application::SetCompatibleTextRenderingDefault(false);
    CSCP2PGUI::Form1 form;
    Application::Run(% form);
    return 0;
}
