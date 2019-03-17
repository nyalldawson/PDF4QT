//    Copyright (C) 2018 Jakub Melka
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

#include "pdfpage.h"
#include "pdfdocument.h"
#include "pdfparser.h"

namespace pdf
{

PDFPageInheritableAttributes PDFPageInheritableAttributes::parse(const PDFPageInheritableAttributes& templateAttributes,
                                                                 const PDFObject& dictionary,
                                                                 const PDFDocument* document)
{
    PDFPageInheritableAttributes result(templateAttributes);

    const PDFObject& dereferencedDictionary = document->getObject(dictionary);
    if (dereferencedDictionary.isDictionary())
    {
        PDFDocumentDataLoaderDecorator loader(document);

        const PDFDictionary* dictionary = dereferencedDictionary.getDictionary();
        if (dictionary->hasKey("MediaBox"))
        {
            result.m_mediaBox = loader.readRectangle(dictionary->get("MediaBox"), result.getMediaBox());
        }
        if (dictionary->hasKey("CropBox"))
        {
            result.m_cropBox = loader.readRectangle(dictionary->get("CropBox"), result.getCropBox());
        }
        if (dictionary->hasKey("Resources"))
        {
            result.m_resources = dictionary->get("Resources");
        }
        if (dictionary->hasKey("Rotate"))
        {
            PDFInteger rotation = loader.readInteger(dictionary->get("Rotate"), 0);

            // PDF specification says, that angle can be multiple of 90, so we can have here
            // for example, 450° (90° * 5), or even negative angles. We must get rid of them.
            PDFInteger fullCircles = rotation / 360;
            if (fullCircles != 0)
            {
                rotation = rotation - fullCircles * 360;
            }

            switch (rotation)
            {
                case 0:
                {
                    result.m_pageRotation = PageRotation::None;
                    break;
                }
                case 90:
                {
                    result.m_pageRotation = PageRotation::Rotate90;
                    break;
                }
                case 180:
                {
                    result.m_pageRotation = PageRotation::Rotate180;
                    break;
                }
                case 270:
                {
                    result.m_pageRotation = PageRotation::Rotate270;
                    break;
                }
                default:
                {
                    throw PDFParserException(PDFTranslationContext::tr("Invalid page rotation."));
                }
            }
        }
    }

    return result;
}

PageRotation PDFPageInheritableAttributes::getPageRotation() const
{
    if (m_pageRotation)
    {
        return m_pageRotation.value();
    }
    return PageRotation::None;
}

std::vector<PDFPage> PDFPage::parse(const PDFDocument* document, const PDFObject& root)
{
    std::vector<PDFPage> result;
    std::set<PDFObjectReference> visited;
    parseImpl(result, visited, PDFPageInheritableAttributes(), root, document);
    return result;
}

QRectF PDFPage::getRectMM(const QRectF& rect) const
{
    return QRectF(convertPDFPointToMM(rect.left()),
                  convertPDFPointToMM(rect.top()),
                  convertPDFPointToMM(rect.width()),
                  convertPDFPointToMM(rect.height()));
}

QRectF PDFPage::getRotatedBox(const QRectF& rect, PageRotation rotation)
{
    switch (rotation)
    {
        case PageRotation::None:
        case PageRotation::Rotate180:
            // Preserve rotation
            break;

        case PageRotation::Rotate90:
        case PageRotation::Rotate270:
            return rect.transposed();
    }

    return rect;
}

void PDFPage::parseImpl(std::vector<PDFPage>& pages,
                        std::set<PDFObjectReference>& visitedReferences,
                        const PDFPageInheritableAttributes& templateAttributes,
                        const PDFObject& root,
                        const PDFDocument* document)
{
    // Are we in internal node, or leaf (page object)?
    const PDFObject& dereferenced = document->getObject(root);

    if (dereferenced.isDictionary())
    {
        const PDFDictionary* dictionary = dereferenced.getDictionary();
        const PDFObject& typeObject =  document->getObject(dictionary->get("Type"));
        if (typeObject.isName())
        {
            PDFPageInheritableAttributes currentInheritableAttributes = PDFPageInheritableAttributes::parse(templateAttributes, root, document);

            QByteArray typeString = typeObject.getString();
            if (typeString == "Pages")
            {
                const PDFObject& kids = document->getObject(dictionary->get("Kids"));
                if (kids.isArray())
                {
                    const PDFArray* kidsArray = kids.getArray();
                    const size_t count = kidsArray->getCount();

                    for (size_t i = 0; i < count; ++i)
                    {
                        const PDFObject& kid = kidsArray->getItem(i);

                        // Check reference
                        if (!kid.isReference())
                        {
                            throw PDFParserException(PDFTranslationContext::tr("Expected valid kids in page tree."));
                        }

                        // Check cycles
                        if (visitedReferences.count(kid.getReference()))
                        {
                            throw PDFParserException(PDFTranslationContext::tr("Detected cycles in page tree."));
                        }

                        visitedReferences.insert(kid.getReference());
                        parseImpl(pages, visitedReferences, currentInheritableAttributes, kid, document);
                    }
                }
                else
                {
                    throw PDFParserException(PDFTranslationContext::tr("Expected valid kids in page tree."));
                }
            }
            else if (typeString == "Page")
            {
                PDFPage page;

                page.m_mediaBox = currentInheritableAttributes.getMediaBox();
                page.m_cropBox = currentInheritableAttributes.getCropBox();
                page.m_resources = document->getObject(currentInheritableAttributes.getResources());
                page.m_pageRotation = currentInheritableAttributes.getPageRotation();

                if (!page.m_cropBox.isValid())
                {
                    page.m_cropBox = page.m_mediaBox;
                }

                PDFDocumentDataLoaderDecorator loader(document);
                page.m_bleedBox = loader.readRectangle(dictionary->get("BleedBox"), page.getCropBox());
                page.m_trimBox = loader.readRectangle(dictionary->get("TrimBox"), page.getCropBox());
                page.m_artBox = loader.readRectangle(dictionary->get("ArtBox"), page.getCropBox());
                page.m_contents = document->getObject(dictionary->get("Contents"));

                pages.emplace_back(std::move(page));
            }
            else
            {
                throw PDFParserException(PDFTranslationContext::tr("Expected valid type item in page tree."));
            }
        }
        else
        {
            throw PDFParserException(PDFTranslationContext::tr("Expected valid type item in page tree."));
        }
    }
    else
    {
        throw PDFParserException(PDFTranslationContext::tr("Expected dictionary in page tree."));
    }
}

}   // namespace pdf