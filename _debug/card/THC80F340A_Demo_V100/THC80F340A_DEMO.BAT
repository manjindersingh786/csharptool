SET PATH=C:\Keil_v5\ARM\ARMCLANG\Bin;C:\windows\system32;C:\windows;C:\windows\system32\Wbem;C:\windows\system32;C:\windows;C:\windows\system32\Wbem;C:\Python310\Scripts\;C:\Python310\;C:\windows\System32\WindowsPowerShell\v1.0\;C:\windows\System32\OpenSSH\;C:\Program Files (x86)\Windows Kits\10\Windows Performance Toolkit\;C:\Program Files\TortoiseGit\bin;C:\Program Files\dotnet\;C:\Users\manjinder.singh\AppData\Local\Microsoft\WindowsApps;C:\Users\manjinder.singh\AppData\Local\GitHubDesktop\bin
SET CPU_TYPE=ARMCM0
SET CPU_VENDOR=ARM
SET UV2_TARGET=THC80F340A_DEMO
SET CPU_CLOCK=0x00B71B00
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmAsm" --Via ".\debug\output\sc000_vect._ia"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmAsm" --Via ".\debug\output\sc000_init._ia"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @".\debug\output\main.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @".\debug\output\memory_api.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @".\debug\output\sc000_isr.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @".\debug\output\sc_api.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @".\debug\output\iso7816_main.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @".\debug\output\iso7816_api.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @".\debug\output\norflash.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmLink" --Via ".\Debug\Output\THC80F340A_SC000_DEMO.lnp"
"C:\Keil_v5\ARM\ARMCLANG\Bin\fromelf.exe" ".\Debug\Output\THC80F340A_SC000_DEMO.axf" --i32combined --output ".\Debug\Output\THC80F340A_SC000_DEMO.hex"
