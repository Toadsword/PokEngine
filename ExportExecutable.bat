@Echo off

:Beginning 
echo Bienvenu dans l'importeur d'exe de PokEngine.
echo Veuillez selectionner :\n
echo 1. PokEngine Editor (Debug)
echo 2. PokEngine Editor (Release)
echo 3. PokEngine Game (Debug)
echo 4. PokEngine Game (Release)
echo 5. Showroom (Debug)
echo 6. Showroom (Release)
echo .
echo 8. Quitter

set /p Choice=Votre choix :
echo Vous avez choisis : %Choice%
if %Choice% LSS 7 goto CreateInstall
if %Choice% == 8 goto End

echo Choix invalide. Reessayez :
goto Beginning

:CreateInstall
mkdir install
if %Choice% == 1 set pokTypeBuild=Debug
if %Choice% == 3 set pokTypeBuild=Debug
if %Choice% == 5 set pokTypeBuild=Debug
if %Choice% == 2 set pokTypeBuild=Release
if %Choice% == 4 set pokTypeBuild=Release
if %Choice% == 6 set pokTypeBuild=Release

if %Choice% == 1 set pokName=PokeEditor%pokTypeBuild%
if %Choice% == 2 set pokName=PokeEditor%pokTypeBuild%
if %Choice% == 3 set pokName=PokeGame%pokTypeBuild%
if %Choice% == 4 set pokName=PokeGame%pokTypeBuild%
if %Choice% == 5 set pokName=PokeShowroom%pokTypeBuild%
if %Choice% == 6 set pokName=PokeShowroom%pokTypeBuild%
mkDir install\%pokName%

set pokFolder=install\%pokName%
echo Copie des dossiers...

xcopy /Q /S /Y AppData %pokFolder%\AppData
echo Dossier 'AppData' copie

xcopy /Q /S /Y Resources %pokFolder%\Resources
echo Dossier 'Ressources' copie

xcopy /Q /S /Y build\x64\%pokTypeBuild% %pokFolder%\bin
echo Dossier 'bin' copie

echo Compression en .zip...
del %pokFolder%_old.zip 
cd install
ren %pokName%.zip %pokName%_old.zip
cd ..
powershell.exe -nologo -noprofile -command "& { Add-Type -A 'System.IO.Compression.FileSystem'; [IO.Compression.ZipFile]::CreateFromDirectory('%pokFolder%', '%pokFolder%.zip'); }"
echo Archive '%pokFolder%.zip' cree !

:End
echo Nettoyage...
set Choice=
set pokName=
set pokTypeBuild=
echo Fini !
pause