###
 ##   Thank you for using the "HerpLog" notetaker, logger and needs-manager
 ##   for your herpetology management requirements. You are looking at the
 ##   source code to make the application work and as such, it will require
 ##   compiling with the appropriate tools.
 ##
 ##
 ##   Copyright (C) 2017-2018. GekkoFyre.
 ##
 ##
 ##   HerpLog is free software: you can redistribute it and/or modify
 ##   it under the terms of the GNU General Public License as published by
 ##   the Free Software Foundation, either version 3 of the License, or
 ##   (at your option) any later version.
 ##
 ##   HerpLog is distributed in the hope that it will be useful,
 ##   but WITHOUT ANY WARRANTY; without even the implied warranty of
 ##   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ##   GNU General Public License for more details.
 ##
 ##   You should have received a copy of the GNU General Public License
 ##   along with HerpLog.  If not, see <http://www.gnu.org/licenses/>.
 ##
 ##
 ##   The latest source code updates can be obtained from [ 1 ] below at your
 ##   leisure. A web-browser or the 'git' application may be required.
 ##
 ##   [ 1 ] - https://github.com/GekkoFyre/HerpLog
 ##
 #################################################################################

# Setup Wizard for `HerpLog` on Microsoft Windows
# Using the NSIS `Modern User Interface`

# --------------------------------
# Includes

!include "LogicLib.nsh"
!include "MUI2.nsh"
!include "x64.nsh"
  
# --------------------------------
# General

!define GK_SETUP_WIZARD_TITLE "${GK_SETUP_WIZARD_TITLE}"

# Name and File
Name "${GK_SETUP_WIZARD_TITLE}"
OutFile "HerpLog_Setup_Win.exe"

# Define installation directory
InstallDir $PROGRAMFILES64

# Get installation folder from registry if available
InstallDirRegKey HKCU "Software\${GK_SETUP_WIZARD_TITLE}" ""

# Request application privileges for Windows Vista
# For removing Start Menu shortcut in Windows 7
RequestExecutionLevel user

# --------------------------------
# Interface Settings

!define MUI_ABORTWARNING

# --------------------------------
# Pages

!define MUI_ICON ".\..\..\ico\setup.ico"
!define MUI_UNICON ".\..\..\ico\uninstall.ico"

!insertmacro MUI_PAGE_LICENSE ".\..\..\..\License.txt"
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
  
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

# --------------------------------
# Languages

!insertmacro MUI_LANGUAGE "English"

# --------------------------------
# Request administrator privileges

Section
    # Call UserInfo plugin to get user info.  The plugin puts the result in the stack
    UserInfo::getAccountType
   
    # Pop the result from the stack into $0
    Pop $0
 
    # Compare the result with the string "Admin" to see if the user is admin.
    # If match, jump 3 lines down.
    StrCmp $0 "Admin" +3
 
    # if there is not a match, print message and return
    MessageBox MB_OK "You must be logged in as an Administrator! Aborting..."
    Return
 
    # otherwise, confirm and return
    MessageBox MB_OK "You are logged in as an Administrator. Proceeding..."
SectionEnd

# --------------------------------
# Installer Sections

!define MSVC_VERSION "14.13.26020"
!define MSVC_DIR ".\..\redist\VCPP\${MSVC_VERSION}"
!define MSVC_REDIST_X64 "VC_redist.x64.exe"
!define MSVC_REDIST_X86 "VC_redist.x86.exe"

# Start default section
Section
    # Set the installation directory as the destination for the following actions
    SetOutPath "$INSTDIR"
    
    # Check if the redistributable(s) are installed or not, and if so, that they are the right version
    Call  CheckRedistributableInstalledX64
    Pop $R0
    ${If} $R0 == "2" # Version currently installed is older than what's provided with the setup wizard
        File "${MSVC_DIR}\${MSVC_REDIST_X64}"
        ExecWait '"$INSTDIR\${MSVC_REDIST_X64}" /passive /norestart'
    ${EndIf}
    
    Call  CheckRedistributableInstalledX86
    Pop $R1
    ${If} $R1 == "2" # Version currently installed is older than what's provided with the setup wizard
        File "${MSVC_DIR}\${MSVC_REDIST_X86}"
        ExecWait '"$INSTDIR\${MSVC_REDIST_X86}" /passive /norestart'
    ${EndIf}
    
    # ADD THE HERPLOG FILES HERE...
    
    # Store installation folder
    WriteRegStr HKCU "Software\${GK_SETUP_WIZARD_TITLE}" "" $INSTDIR
    
    # Create the uninstaller
    WriteUninstaller "$INSTDIR\uninstall.exe"
    
    # Create a shortcut named "Uninstall HerpLog" in the start menu programs directory
    # Point the new shortcut at the program uninstaller
    CreateShortCut "$SMPROGRAMS\Uninstall HerpLog.lnk" "$INSTDIR\uninstall.exe"
SectionEnd

# --------------------------------
# Descriptions

#Language strings
LangString DESC_SecDummy ${LANG_ENGLISH} "Description goes here."

;Assign language strings to sections
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${SecDummy} $(DESC_SecDummy)
!insertmacro MUI_FUNCTION_DESCRIPTION_END

# --------------------------------
# Uninstaller section

Section "uninstall"
    # ADD HERPLOG'S FILES HERE...

    # First, delete the uninstaller
    Delete "$INSTDIR\uninstall.exe"
    
    # Second, remove the link from the start menu
    Delete "$SMPROGRAMS\Uninstall HerpLog.lnk"
    
    RMDir "$INSTDIR"
    
    DeleteRegKey /ifempty HKCU "Software\${GK_SETUP_WIZARD_TITLE}"
SectionEnd

# --------------------------------
# A function that compares two version numbers
# Author: http://nsis.sourceforge.net/VersionCheckNew:_Compare_version_numbers

!macro VersionCheck Ver1 Ver2 OutVar
    Push "${Ver1}"
    Push "${Ver2}"
        Call VersionCheck
    Pop "${OutVar}"
!macroend

!define VersionCheckNew "!insertmacro VersionCheck"

Function VersionCheck
    Exch $R0 # Second version number
    Exch
    Exch $R1 # First version number
    Push $R2
    Push $R3
    Push $R4
    Push $R5 # Second version part
    Push $R6 # First version part
    
        StrCpy $R1 $R1.
        StrCpy $R0 $R0.
        
    Next: StrCmp $R0$R1 "" 0 +3
        StrCpy $R0 0
        Goto Done
        
        StrCmp $R0 "" 0 +2
            StrCpy $R0 0.
        StrCmp $R1 "" 0 +2
            StrCpy $R1 0.
    
    StrCpy $R2 0
        IntOp $R2 $R2 + 1
        StrCpy $R4 $R1 1 $R2
        StrCmp $R4 . 0 -2
            StrCpy $R6 $R1 $R2
            IntOp $R2 $R2 + 1
            StrCpy $R1 $R1 "" $R2
            
    StrCpy $R2 0
        IntOp $R2 $R2 + 1
        StrCpy $R4 $R0 1 $R2
        StrCmp $R4 . 0 -2
            StrCpy $R5 $R0 $R2
            IntOp $R2 $R2 + 1
            StrCpy $R0 $R0 "" $R2

    IntCmp $R5 0 Compare
    IntCmp $R6 0 Compare
    
    StrCpy $R3 0
        StrCpy $R4 $R6 1 $R3
        IntOp $R3 $R3 + 1
        StrCmp $R4 0 -2
        
    StrCpy $R2 0
        StrCpy $R4 $R5 1 $R2
        IntOp $R2 $R2 + 1
        StrCmp $R4 0 -2
        
    IntCmp $R3 $R2 0 +2 +4
    Compare: IntCmp 1$R5 1$R6 Next 0 +3
    
        StrCpy $R0 1
        Goto Done
        StrCpy $R0 2
        
    Done:
    Pop $R6
    Pop $R5
    Pop $R4
    Pop $R3
    Pop $R2
    Pop $R1
    Exch $R0 # Output
FunctionEnd

# --------------------------------
# Checks the version of the Visual C++ Runtime that's presently installed

Function CheckRedistributableInstalledX64
    # {7474cd6e-76cc-4257-837e-5b9261e526af} - Microsoft Visual C++ 2017 Redistributable (x64) - 14.13.26020
    Push $R0
    ClearErrors
    
    # Try to read the version sub-key to `R0`
    ReadRegDword $R0 HKLM "Computer\HKEY_CLASSES_ROOT\Installer\Dependencies\,,amd64,14.0,bundle" "Version"
    
    # Was there error or not?
    IfErrors 0 NoErrors
    
    # Error occurred, copy "Error" to R0
    StrCpy $R0 "Error"
    
    NoErrors:
        ${VersionCheckNew} "$R0" ${MSVC_VERSION} "$R1"
        ${If} $R1 == "2" # Version currently installed is older than what's provided with the setup wizard
            StrCpy $R1 $R0
        ${Else}
            StrCpy $R0 "0" # Version currently installed is equal or newer to what's provided with the setup wizard
            
        Exch $R0
FunctionEnd

Function CheckRedistributableInstalledX86
    # {5c045b7f-e561-4794-91f8-c6cda0893107} - Microsoft Visual C++ 2017 Redistributable (x86) - 14.13.26020
    Push $R0
    ClearErrors
    
    # Try to read the version sub-key to `R0`
    ReadRegDword $R0 HKLM "Computer\HKEY_CLASSES_ROOT\Installer\Dependencies\,,x86,14.0,bundle" "Version"
    
    # Was there error or not?
    IfErrors 0 NoErrors
    
    # Error occurred, copy "Error" to R0
    StrCpy $R0 "Error"
    
    NoErrors:
        ${VersionCheckNew} "$R0" ${MSVC_VERSION} "$R1"
        ${If} $R1 == "2" # Version currently installed is older than what's provided with the setup wizard
            StrCpy $R1 $R0
        ${Else}
            StrCpy $R0 "0" # Version currently installed is equal or newer to what's provided with the setup wizard
        ${EndIf}
            
        Exch $R0
FunctionEnd

# --------------------------------