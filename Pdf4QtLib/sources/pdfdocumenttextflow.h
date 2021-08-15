//    Copyright (C) 2020-2021 Jakub Melka
//
//    This file is part of PDF4QT.
//
//    PDF4QT is free software: you can redistribute it and/or modify
//    it under the terms of the GNU Lesser General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    with the written consent of the copyright owner, any later version.
//
//    PDF4QT is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU Lesser General Public License for more details.
//
//    You should have received a copy of the GNU Lesser General Public License
//    along with PDF4QT.  If not, see <https://www.gnu.org/licenses/>.

#ifndef PDFDOCUMENTTEXTFLOW_H
#define PDFDOCUMENTTEXTFLOW_H

#include "pdfglobal.h"
#include "pdfexception.h"

namespace pdf
{
class PDFDocument;

/// Text flow extracted from document. Text flow can be created \p PDFDocumentTextFlowFactory.
/// Flow can contain various items, not just text ones. Also, some manipulation functions
/// are available, they can modify text flow by various content.
class PDF4QTLIBSHARED_EXPORT PDFDocumentTextFlow
{
public:

    enum Flag
    {
        None                                = 0x0000,   ///< No text flag
        Text                                = 0x0001,   ///< Ordinary text
        PageStart                           = 0x0002,   ///< Page start marker
        PageEnd                             = 0x0004,   ///< Page end marker
        StructureTitle                      = 0x0008,   ///< Structure tree item title
        StructureLanguage                   = 0x0010,   ///< Structure tree item language
        StructureAlternativeDescription     = 0x0020,   ///< Structure tree item alternative description
        StructureExpandedForm               = 0x0040,   ///< Structure tree item expanded form of text
        StructureActualText                 = 0x0080,   ///< Structure tree item actual text
        StructurePhoneme                    = 0x0100,   ///< Structure tree item  phoneme
        StructureItemStart                  = 0x0200,   ///< Start of structure tree item
        StructureItemEnd                    = 0x0400,   ///< End of structure tree item
    };
    Q_DECLARE_FLAGS(Flags, Flag)

    struct Item
    {
        QRectF boundingRect; ///< Bounding rect in page coordinates
        PDFInteger pageIndex = 0;
        QString text;
        Flags flags = None;
    };
    using Items = std::vector<Item>;

    explicit PDFDocumentTextFlow() = default;
    explicit PDFDocumentTextFlow(Items&& items) :
        m_items(qMove(items))
    {

    }

    const Items& getItems() const { return m_items; }

    /// Returns item at a given index
    /// \param index Index
    const Item* getItem(size_t index) const { return &m_items.at(index); }

    /// Returns text flow item count
    size_t getSize() const { return m_items.size(); }

    /// Returns true, if text flow is empty
    bool isEmpty() const { return m_items.empty(); }

private:
    Items m_items;
};

/// This factory creates text flow for whole document
class PDF4QTLIBSHARED_EXPORT PDFDocumentTextFlowFactory
{
public:
    explicit PDFDocumentTextFlowFactory() = default;

    enum class Algorithm
    {
        Auto,       ///< Determine best text layout algorithm automatically
        Layout,     ///< Use text layout recognition using docstrum algorithm
        Content,    ///< Use content-stream text layout recognition (usually unreliable), but fast
        Structure,  ///< Use structure oriented text layout recognition (requires tagged document)
    };

    /// Performs document text flow analysis using given algorithm. Text flow
    /// can be performed only for given subset of pages, if required.
    /// \param document Document
    /// \param pageIndices Analyzed page indices
    /// \param algorithm Algorithm
    PDFDocumentTextFlow create(const PDFDocument* document,
                               const std::vector<PDFInteger>& pageIndices,
                               Algorithm algorithm);

    /// Performs document text flow analysis using given algorithm. Text flow
    /// is created for all pages.
    /// \param document Document
    /// \param algorithm Algorithm
    PDFDocumentTextFlow create(const PDFDocument* document, Algorithm algorithm);

    /// Has some error/warning occured during text layout creation?
    bool hasError() const { return !m_errors.isEmpty(); }

    /// Returns a list of errors/warnings
    const QList<PDFRenderError>& getErrors() const { return m_errors; }

    /// Sets if bounding boxes for text blocks should be calculated
    /// \param calculateBoundingBoxes Perform bounding box calculation?
    void setCalculateBoundingBoxes(bool calculateBoundingBoxes);

private:
    QList<PDFRenderError> m_errors;
    bool m_calculateBoundingBoxes = false;
};

/// Editor which can edit document text flow, modify user text,
/// change order of text items, restore original state of a text flow,
/// and many other features.
class PDF4QTLIBSHARED_EXPORT PDFDocumentTextFlowEditor
{
public:
    inline PDFDocumentTextFlowEditor() = default;

    /// Sets a text flow and initializes edited text flow
    /// \param textFlow Text flow
    void setTextFlow(PDFDocumentTextFlow textFlow);

    void removeItem(size_t index);
    void addItem(size_t index);

    void clear();

    enum EditedItemFlag
    {
        None        = 0x0000,
        Removed     = 0x0001,
        Modified    = 0x0002
    };
    Q_DECLARE_FLAGS(EditedItemFlags, EditedItemFlag)

    struct EditedItem : public PDFDocumentTextFlow::Item
    {
        size_t originalIndex = 0; ///< Index of original item
        EditedItemFlags editedItemFlags = None;
    };

    using EditedItems = std::vector<EditedItem>;

    /// Returns true, if item is active
    /// \param index Index
    bool isActive(size_t index) const { return !getEditedItem(index)->editedItemFlags.testFlag(Removed); }

    /// Returns true, if item is removed
    /// \param index Index
    bool isRemoved(size_t index) const { return !isActive(index); }

    /// Returns true, if item is modified
    /// \param index Index
    bool isModified(size_t index) const { return getEditedItem(index)->editedItemFlags.testFlag(Modified); }

    /// Returns edited text (or original, if edited text is not modified)
    /// for a given index.
    /// \param index Index
    const QString& getText(size_t index) const { return getEditedItem(index)->text; }

    /// Sets edited text for a given index
    void setText(const QString& text, size_t index);

    /// Returns true, if text flow is empty
    bool isEmpty() const { return m_originalTextFlow.isEmpty(); }

private:
    void createEditedFromOriginalTextFlow();
    void updateModifiedFlag(size_t index);

    const PDFDocumentTextFlow::Item* getOriginalItem(size_t index) const { return m_originalTextFlow.getItem(index); }
    EditedItem* getEditedItem(size_t index) { return &m_editedTextFlow.at(index); }
    const EditedItem* getEditedItem(size_t index) const { return &m_editedTextFlow.at(index); }

    PDFDocumentTextFlow m_originalTextFlow;
    EditedItems m_editedTextFlow;
};

}   // namespace pdf

#endif // PDFDOCUMENTTEXTFLOW_H
