// Folgender ifdef-Block ist die Standardmethode zum Erstellen von Makros, die das Exportieren 
// aus einer DLL vereinfachen. Alle Dateien in dieser DLL werden mit dem MSCOREE_FORWARD_EXPORTS-Symbol
// kompiliert, das in der Befehlszeile definiert wurde. Das Symbol darf nicht für ein Projekt definiert werden,
// das diese DLL verwendet. Alle anderen Projekte, deren Quelldateien diese Datei beinhalten, erkennen 
// MSCOREE_FORWARD_API-Funktionen als aus einer DLL importiert, während die DLL
// mit diesem Makro definierte Symbole als exportiert ansieht.
#ifdef MSCOREE_FORWARD_EXPORTS
#define MSCOREE_FORWARD_API __declspec(dllexport)
#else
#define MSCOREE_FORWARD_API __declspec(dllimport)
#endif



// class name and dll-name of the plugin to be hosted
#define myCOMclass "myCompany.myPlugin"
#define myCOMdll "AVRStudioPlugin.dll"
