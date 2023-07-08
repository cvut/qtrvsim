/****************************************************************************
**
** Copyright (C) 2018 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qhtml5file.h"

#include <emscripten.h>
#include <emscripten/html5.h>
#include <qdebug.h>

//
// This file implements file loading via the HTML file input element and file save via
// browser download.
//

// Global user file data ready callback and C helper function. JavaScript will
// call this function when the file data is ready; the helper then forwards
// the call to the current handler function. This means there can be only one
// file open in proress at a given time.
std::function<void(char *, size_t, const char *)> g_qtFileDataReadyCallback;
extern "C" EMSCRIPTEN_KEEPALIVE void
qt_callFileDataReady(char *content, size_t contentSize, const char *fileName) {
    if (g_qtFileDataReadyCallback == nullptr) {
        return;
    }

    g_qtFileDataReadyCallback(content, contentSize, fileName);
    g_qtFileDataReadyCallback = nullptr;
}

namespace {
void loadFile(
    const char *accept,
    std::function<void(char *, size_t, const char *)> fileDataReady) {
    if (::g_qtFileDataReadyCallback) {
        puts("Warning: Concurrent loadFile() calls are not supported. "
             "Cancelling earlier call");
    }

    // Call qt_callFileDataReady to make sure the emscripten linker does not
    // optimize it away, which may happen if the function is called from
    // JavaScript only. Set g_qtFileDataReadyCallback to null to make it a a
    // no-op.
    ::g_qtFileDataReadyCallback = nullptr;
    ::qt_callFileDataReady(nullptr, 0, nullptr);

    ::g_qtFileDataReadyCallback = fileDataReady;
    EM_ASM_(
        {
            const accept = UTF8ToString($0);

            // Crate file file input which whil display the native file dialog
            var fileElement = document.createElement("input");
            document.body.appendChild(fileElement);
            fileElement.type = "file";
            fileElement.style = "display:none";
            fileElement.accept = accept;
            fileElement.onchange = function(event) {
                const files = event.target.files;

                // Read files
                for (var i = 0; i < files.length; i++) {
                    const file = files[i];
                    var reader = new FileReader();
                    reader.onload = function() {
                        const name = file.name;
                        var contentArray = new Uint8Array(reader.result);
                        const contentSize = reader.result.byteLength;

                        // Copy the file file content to the C++ heap.
                        // Note: this could be simplified by passing the content
                        // as an "array" type to ccall and then let it copy to
                        // C++ memory. However, this built-in solution does not
                        // handle files larger than ~15M (Chrome). Instead,
                        // allocate memory manually and pass a pointer to the
                        // C++ side (which will free() it when done).

                        // TODO: consider slice()ing the file to read it
                        // picewise and then assembling it in a QByteArray on
                        // the C++ side.

                        const heapPointer = _malloc(contentSize);
                        const heapBytes = new Uint8Array(
                            Module.HEAPU8.buffer, heapPointer, contentSize);
                        heapBytes.set(contentArray);

                        // Null out the first data copy to enable GC
                        reader = null;
                        contentArray = null;

                        // Call the C++ file data ready callback
                        ccall(
                            "qt_callFileDataReady", null,
                            [ "number", "number", "string" ],
                            [ heapPointer, contentSize, name ]);
                    };
                    reader.readAsArrayBuffer(file);
                }

                // Clean up document
                document.body.removeChild(fileElement);

            }; // onchange callback

            // Trigger file dialog open
            fileElement.click();
        },
        accept);
}

void saveFile(
    const char *contentPointer,
    size_t contentLength,
    const char *fileNameHint) {
    EM_ASM_(
        {
            // Make the file contents and file name hint accessible to
            // Javascript: convert the char * to a JavaScript string and create
            // a subarray view into the C heap.
            const contentPointer = $0;
            const contentLength = $1;
            const fileNameHint = UTF8ToString($2);
            const fileContent = Module.HEAPU8.subarray(
                contentPointer, contentPointer + contentLength);

            // Create a hidden download link and click it programatically
            const fileblob = new Blob([fileContent], {
                type:
                    "application/octet-stream"
            });
            var link = document.createElement("a");
            document.body.appendChild(link);
            link.download = fileNameHint;
            link.href = window.URL.createObjectURL(fileblob);
            link.style = "display:none";
            link.click();
            document.body.removeChild(link);
        },
        contentPointer, contentLength, fileNameHint);
}
} // namespace

/*!
    \brief Read local file via file dialog.

    Call this function to make the browser display an open-file dialog. This
   function returns immediately, and \a fileDataReady is called when the user
   has selected a file and the file contents has been read.

    \a The accept argument specifies which file types to accept, and must follow
   the <input type="file"> html standard formatting, for example ".png, .jpg,
   .jpeg".

    This function is implemented on Qt for WebAssembly only. A nonfunctional
   cross- platform stub is provided so that code that uses it can compile on all
   platforms.
*/
void QHtml5File::load(
    const QString &accept,
    std::function<void(const QByteArray &, const QString &)> fileDataReady) {
    loadFile(
        accept.toUtf8().constData(),
        [=](char *content, size_t size, const char *fileName) {
            // Copy file data into QByteArray and free buffer that was allocated
            // on the JavaScript side. We could have used
            // QByteArray::fromRawData() to avoid the copy here, but that would
            // make memory management awkward.
            QByteArray qtFileContent(content, size);
            free(content);

            // Call user-supplied data ready callback
            fileDataReady(qtFileContent, QString::fromUtf8(fileName));
        });
}

/*!
    \brief Write local file via browser download

    Call this function to make the browser start a file download. The file
    will contains the given \a content, with a suggested \a fileNameHint.

    This function is implemented on Qt for WebAssembly only. A nonfunctional
   cross- platform stub is provided so that code that uses it can compile on all
   platforms.
*/
void QHtml5File::save(const QByteArray &content, const QString &fileNameHint) {
    // Convert to C types and save
    saveFile(
        content.constData(), content.size(), fileNameHint.toUtf8().constData());
}
