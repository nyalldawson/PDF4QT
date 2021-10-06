
# PDF4QT
**(c) Jakub Melka 2018-2021**

**Mgr.Jakub.Melka@gmail.com**

This software is consisting of PDF rendering library, and several
applications, such as advanced document viewer, command line tool,
and document page manipulator application. Software is implementing PDF
functionality based on PDF Reference 2.0. It is written and maintained
by Jakub Melka.

*Software works only on Microsoft Windows.*

Software is provided without any warranty of any kind

## 1. ACKNOWLEDGEMENTS

This software is based in part on the work of the Independent JPEG Group.

Portions of this software are copyright © 2019 The FreeType
Project (www.freetype.org). All rights reserved.

## 2. LEGAL ISSUES

Both library and viewer uses more benevolent LGPL license, so it is more
usable in commercial software, than GPL code only. Please see attached
file - LICENSE.txt to see details. This software also uses several
third party software, and user of this software must also respect licenses
of third party libraries.

## 3. FEATURES

Software have following features (the list is not complete):

- [x] multithreading support
- [x] hardware accelerated rendering
- [x] encryption
- [x] color management
- [x] optional content handling
- [x] text layout analysis
- [x] signature validation
- [x] annotations
- [x] form filling
- [x] text to speech capability
- [x] editation
- [x] file attachments
- [x] optimalization (compressing documents)
- [x] command line tool
- [x] audio book conversion
- [x] internal structure inspector
- [ ] compare documents *(planned in year 2021)*
- [ ] XFA support *(planned in year 2022)*
- [ ] create fillable forms *(planned in year 2022)*
- [ ] electronically/digitally sign documents *(planned in year 2023)*
- [ ] 3D PDF support *(planned in year 2023)*
- [ ] watermarks / headers / footers *(planned in year 2023)*
- [ ] presentation application *(planned in year 2023)*
- [ ] public key security encryption *(planned in year 2023)*

## 4. THIRD PARTY LIBRARIES

Several third-party libraries are used.

1. libjpeg, see https://www.ijg.org/
2. FreeType, see https://www.freetype.org/index.html, FTL license used
3. OpenJPEG, implementing Jpeg2000, see https://www.openjpeg.org/, 2-clause MIT license
4. Qt, https://www.qt.io/, LGPL license used
5. OpenSSL, https://www.openssl.org/, Apache 2.0 license
6. LittleCMS, http://www.littlecms.com/
7. zlib, https://zlib.net/

## 5. CONTRIBUTIONS

If you want to contribute to this project, it is required, that you (contributor)
fill and digitally sign document [Contributor License Agreement](CLA/Contributor_License_Agreement.pdf),
because I want to have a freedom to do whatever I want with my library, without obligation
to someone else. But I would strongly prefer, if you want to contribute, to contribute
in a form of testing, consultation, giving advices etc. I would like to write this library
entirely by myself.

## 6. COMPILING

To compile this project, Visual Studio 2019 is needed. Only Microsoft Windows
is supported at this time. You must also have all of above libraries in order
to compile the project. Software uses Qt 5.14.2.

### Compilation instructions:
1. Download Qt 5.14.2
2. Download [precompiled libraries](https://github.com/JakubMelka/PdfForQt-Dependencies),
   or compile them yourself. Libraries must be in same root directory as this project,
   so root folder of this project will have a sibling folder with these libraries
3. Open Qt Creator and root project Pdf4Qt.pro
4. Create target for Microsoft Visual Studio 2019 and compile the project

## 7. DISCLAIMER

I wrote this project in my free time. I hope you will find it useful!