//    Copyright (C) 2019 Jakub Melka
//
//    This file is part of PdfForQt.
//
//    PdfForQt is free software: you can redistribute it and/or modify
//    it under the terms of the GNU Lesser General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    PdfForQt is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU Lesser General Public License for more details.
//
//    You should have received a copy of the GNU Lesser General Public License
//    along with PDFForQt.  If not, see <https://www.gnu.org/licenses/>.

#include "pdfrenderer.h"
#include "pdfpainter.h"
#include "pdfdocument.h"

namespace pdf
{

PDFRenderer::PDFRenderer(const PDFDocument* document) :
    m_document(document),
    m_features(Antialiasing | TextAntialiasing)
{
    Q_ASSERT(document);
}

// TODO: Dodelat features, napr. antialiasing
// TODO: Dodelat rotovani stranek
// TODO: Dodelat obrazky

QList<PDFRenderError> PDFRenderer::render(QPainter* painter, const QRectF& rectangle, size_t pageIndex) const
{
    Q_UNUSED(painter);
    Q_UNUSED(rectangle);

    const PDFCatalog* catalog = m_document->getCatalog();
    if (pageIndex >= catalog->getPageCount() || !catalog->getPage(pageIndex))
    {
        // Invalid page index
        return { PDFRenderError(RenderErrorType::Error, PDFTranslationContext::tr("Page %1 doesn't exist.").arg(pageIndex + 1)) };
    }

    const PDFPage* page = catalog->getPage(pageIndex);
    Q_ASSERT(page);

    QRectF mediaBox = page->getMediaBox();

    QMatrix matrix;
    matrix.translate(rectangle.left(), rectangle.bottom());
    matrix.scale(rectangle.width() / mediaBox.width(), -rectangle.height() / mediaBox.height());

    PDFPainter processor(painter, m_features, matrix, page, m_document);
    return processor.processContents();
}

QList<PDFRenderError> PDFRenderer::render(QPainter* painter, const QMatrix& matrix, size_t pageIndex) const
{
    Q_UNUSED(painter);

    const PDFCatalog* catalog = m_document->getCatalog();
    if (pageIndex >= catalog->getPageCount() || !catalog->getPage(pageIndex))
    {
        // Invalid page index
        return { PDFRenderError(RenderErrorType::Error, PDFTranslationContext::tr("Page %1 doesn't exist.").arg(pageIndex + 1)) };
    }

    const PDFPage* page = catalog->getPage(pageIndex);
    Q_ASSERT(page);

    PDFPainter processor(painter, m_features, matrix, page, m_document);
    return processor.processContents();
}

}   // namespace pdf