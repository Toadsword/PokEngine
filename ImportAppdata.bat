@Echo off

:Beginning 

REM Read local config
for /f "delims=" %%a in (appdataConfig.txt) DO ( 
  set PokPath=%%a
)
if "%PokPath%" == "" goto BadFile
echo Path touvé : %PokPath%

REM echo Importer ou exporter Appdata ?
set /p Choice=Importer de '%PokPath%' vers 'Appdata\resources'? (y/n):

if "%Choice%" == "y" goto Import
if "%Choice%" == "n" goto End

goto End

:Import
echo Copie de '%PokPath%' vers 'Appdata\ressources'
xcopy /Q /S /Y %PokPath% Appdata\resources
goto End

REM :Export

REM echo Copie de 'Appdata\ressources'  vers '%PokPath%'
REM xcopy /Q /S /Y Appdata\resources '%PokPath%'

:BadFile
echo .
echo Fichier de configuration 'appdata.txt' non trouve
echo C:\Users\Toaduru\Documents\Resilio\Pok_Resources>appdataConfig.txt
echo Pour le bon fonctionnement du tool :
echo    1. Un fichier appdataconfig.txt a ete cree
echo    2. Notez dedans le path de votre dossier resilio,
echo           par exemple : C:\Users\Toaduru\Documents\Resilio\Pok_Resources
echo    3. sauvegardez le fichier et 
echo    4. relancez le tool
echo .
echo Si, quand le tool est lance, trouve le bon chemin de fichier, il pourra faire l'import des fichiers du resilio vers Appdata/resources
echo .
echo En cas de doute, veuillez contacter votre Devops
echo .
echo . 

:End
echo Nettoyage...
set Choice=
set PokPath=
echo Fini !
pause