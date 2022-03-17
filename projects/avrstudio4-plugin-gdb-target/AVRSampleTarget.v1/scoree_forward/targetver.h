#pragma once

// Die folgenden Makros definieren die mindestens erforderliche Plattform. Die mindestens erforderliche Plattform
// ist die fr�heste Windows-, Internet Explorer-Version usw., die �ber die erforderlichen Features zur Ausf�hrung 
// Ihrer Anwendung verf�gt. Die Makros aktivieren alle Funktionen, die auf den Plattformversionen bis 
// einschlie�lich der angegebenen Version verf�gbar sind.

// �ndern Sie folgende Definitionen f�r Plattformen, die �lter als die unten angegebenen sind.
// Unter MSDN finden Sie die neuesten Informationen �ber die entsprechenden Werte f�r die unterschiedlichen Plattformen.
#ifndef WINVER                          // Gibt an, dass Windows Vista die mindestens erforderliche Plattform ist.
#define WINVER 0x0600           // �ndern Sie den entsprechenden Wert, um auf andere Versionen von Windows abzuzielen.
#endif

#ifndef _WIN32_WINNT            // Gibt an, dass Windows Vista die mindestens erforderliche Plattform ist.
#define _WIN32_WINNT 0x0600     // �ndern Sie den entsprechenden Wert, um auf andere Versionen von Windows abzuzielen.
#endif

#ifndef _WIN32_WINDOWS          // Gibt an, dass Windows 98 die mindestens erforderliche Plattform ist.
#define _WIN32_WINDOWS 0x0410 // �ndern Sie den entsprechenden Wert, um auf mindestens Windows Me abzuzielen.
#endif

#ifndef _WIN32_IE                       // Gibt an, dass Internet Explorer 7.0 die mindestens erforderliche Plattform ist.
#define _WIN32_IE 0x0700        // �ndern Sie den entsprechenden Wert, um auf andere Versionen von IE abzuzielen.
#endif
