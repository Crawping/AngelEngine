// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved
//
// Contents:    Extended TextLayout that permits editing.
//
// Remarks:     Internally, a new DirectWrite layout is recreated when the
//              text is edited, but the caller can safely hold the same
//              reference, since the adapter forwards all the calls onward.
//
//----------------------------------------------------------------------------
// C headers:
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <utility>
#include <math.h>

// C++ headers:
#include <algorithm>

#include <limits>
#include <memory>
#include <new>
#include <numeric>
#include <string>
#include <vector>

// Windows headers:

#include <windows.h>
#include <windowsx.h>
#include <winnls.h>
#include <unknwn.h>

#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>
#include <strsafe.h>
#include <commdlg.h>
#include"GlobalTools.h"
#include "EditableLayout.h"


HRESULT EditableLayout::RecreateLayout(
    IN OUT IDWriteTextLayout*& currentLayout,
    const std::wstring& text
    )
{
    // Recreates the internally held layout.

    HRESULT hr = S_OK;

    IDWriteTextLayout* newLayout = NULL;

    hr = factory_->CreateTextLayout(
            text.c_str(),
            static_cast<UINT32>(text.length()),
            currentLayout,
            currentLayout->GetMaxWidth(),
            currentLayout->GetMaxHeight(),
            &newLayout
            );

    if (SUCCEEDED(hr))
		AngelGUISystem::GUIGlobalTools::SafeAttach(&currentLayout, AngelGUISystem::GUIGlobalTools::SafeDetach(&newLayout));

	AngelGUISystem::GUIGlobalTools::SafeRelease(&newLayout);

    return hr;
}


void EditableLayout::CopySinglePropertyRange(
    IDWriteTextLayout* oldLayout,
    UINT32 startPosForOld,
    IDWriteTextLayout* newLayout,
    UINT32 startPosForNew,
    UINT32 length,
    EditableLayout::CaretFormat* caretFormat
    )
{
    // Copies a single range of similar properties, from one old layout
    // to a new one.

    DWRITE_TEXT_RANGE range = {startPosForNew,  DirectX::XMMin<UINT32>(length, UINT32_MAX - startPosForNew)};

    // font collection
    IDWriteFontCollection* fontCollection = NULL;
    oldLayout->GetFontCollection(startPosForOld, &fontCollection);
    newLayout->SetFontCollection(fontCollection, range);
	AngelGUISystem::GUIGlobalTools::SafeRelease(&fontCollection);

    if (caretFormat != NULL)
    {
        newLayout->SetFontFamilyName(caretFormat->fontFamilyName, range);
        newLayout->SetLocaleName(caretFormat->localeName, range);
        newLayout->SetFontWeight(caretFormat->fontWeight, range);
        newLayout->SetFontStyle(caretFormat->fontStyle, range);
        newLayout->SetFontStretch(caretFormat->fontStretch, range);
        newLayout->SetFontSize(caretFormat->fontSize, range);
        newLayout->SetUnderline(caretFormat->hasUnderline, range);
        newLayout->SetStrikethrough(caretFormat->hasStrikethrough, range);
    }
    else
    {
        // font family
        wchar_t fontFamilyName[100];
        fontFamilyName[0] = '\0';
        oldLayout->GetFontFamilyName(startPosForOld, &fontFamilyName[0], ARRAYSIZE(fontFamilyName));
        newLayout->SetFontFamilyName(fontFamilyName, range);

        // weight/width/slope
        DWRITE_FONT_WEIGHT weight   = DWRITE_FONT_WEIGHT_NORMAL;
        DWRITE_FONT_STYLE style     = DWRITE_FONT_STYLE_NORMAL;
        DWRITE_FONT_STRETCH stretch = DWRITE_FONT_STRETCH_NORMAL;
        oldLayout->GetFontWeight(startPosForOld, &weight);
        oldLayout->GetFontStyle(startPosForOld, &style);
        oldLayout->GetFontStretch(startPosForOld, &stretch);

        newLayout->SetFontWeight(weight, range);
        newLayout->SetFontStyle(style, range);
        newLayout->SetFontStretch(stretch, range);

        // font size
        FLOAT fontSize = 12.0;
        oldLayout->GetFontSize(startPosForOld, &fontSize);
        newLayout->SetFontSize(fontSize, range);

        // underline and strikethrough
        BOOL value = FALSE;
        oldLayout->GetUnderline(startPosForOld,&value);
        newLayout->SetUnderline(value,range);
        oldLayout->GetStrikethrough(startPosForOld, &value);
        newLayout->SetStrikethrough(value, range);

        // locale
        wchar_t localeName[LOCALE_NAME_MAX_LENGTH];
        localeName[0] = '\0';
        oldLayout->GetLocaleName(startPosForOld, &localeName[0], ARRAYSIZE(localeName));
        newLayout->SetLocaleName(localeName, range);
    }

    // drawing effect
    IUnknown* drawingEffect = NULL;
    oldLayout->GetDrawingEffect(startPosForOld, &drawingEffect);
    newLayout->SetDrawingEffect(drawingEffect, range);
	AngelGUISystem::GUIGlobalTools::SafeRelease(&drawingEffect);

    // inline object
    IDWriteInlineObject* inlineObject = NULL;
    oldLayout->GetInlineObject(startPosForOld, &inlineObject);
    newLayout->SetInlineObject(inlineObject, range);
	AngelGUISystem::GUIGlobalTools::SafeRelease(&inlineObject);

    // typography
    IDWriteTypography* typography = NULL;
    oldLayout->GetTypography(startPosForOld, &typography);
    newLayout->SetTypography(typography, range);
	AngelGUISystem::GUIGlobalTools::SafeRelease(&typography);
}


UINT32 CalculateRangeLengthAt(
    IDWriteTextLayout* layout,
    UINT32 pos
    )
{
    // Determines the length of a block of similarly formatted properties.

    // Use the first getter to get the range to increment the current position.
    DWRITE_TEXT_RANGE incrementAmount = {pos, 1};
    DWRITE_FONT_WEIGHT weight = DWRITE_FONT_WEIGHT_NORMAL;

    layout->GetFontWeight(
        pos,
        &weight,
        &incrementAmount
        );

    UINT32 rangeLength = incrementAmount.length - (pos - incrementAmount.startPosition);
    return rangeLength;
}


void EditableLayout::CopyRangedProperties(
    IDWriteTextLayout* oldLayout,
    UINT32 startPos,
    UINT32 endPos, // an STL-like one-past position.
    UINT32 newLayoutTextOffset,
    IDWriteTextLayout* newLayout,
    bool isOffsetNegative
    )
{
    // Copies properties that set on ranges.

    UINT32 currentPos = startPos;
    while (currentPos < endPos)
    {
        UINT32 rangeLength = CalculateRangeLengthAt(oldLayout, currentPos);
        rangeLength = DirectX::XMMin<UINT32>(rangeLength, endPos - currentPos);
        if (isOffsetNegative)
        {
            CopySinglePropertyRange(
                oldLayout,
                currentPos,
                newLayout,
                currentPos - newLayoutTextOffset,
                rangeLength
                );
        }
        else
        {
            CopySinglePropertyRange(
                oldLayout,
                currentPos,
                newLayout,
                currentPos + newLayoutTextOffset,
                rangeLength
                );
        }
        currentPos += rangeLength;
    }
}


STDMETHODIMP EditableLayout::InsertTextAt(
    IN OUT IDWriteTextLayout*& currentLayout,
    IN OUT std::wstring& text,
    UINT32 position,
    WCHAR const* textToInsert,                  // [lengthToInsert]
    UINT32 textToInsertLength,
    CaretFormat* caretFormat
    )
{
    // Inserts text and shifts all formatting.

    HRESULT hr = S_OK;

    // The inserted string gets all the properties of the character right before position.
    // If there is no text right before position, so use the properties of the character right after position.

    // Copy all the old formatting.
    IDWriteTextLayout* oldLayout = AngelGUISystem::GUIGlobalTools::SafeAcquire(currentLayout);
    UINT32 oldTextLength = static_cast<UINT32>(text.length());
    position = DirectX::XMMin<UINT32>(position, static_cast<UINT32>(text.size()));

   
       // Insert the new text and recreate the new text layout.
    text.insert(position, textToInsert, textToInsertLength);
    if (SUCCEEDED(hr))
    {
        hr = RecreateLayout(currentLayout, text);
    }

    IDWriteTextLayout* newLayout = currentLayout;

    if (SUCCEEDED(hr))
    {
        CopyGlobalProperties(oldLayout, newLayout);

        // For each property, get the position range and apply it to the old text.
        if (position == 0)
        {
            // Inserted text
            CopySinglePropertyRange(oldLayout, 0, newLayout, 0, textToInsertLength);

            // The rest of the text
            CopyRangedProperties(oldLayout, 0, oldTextLength, textToInsertLength, newLayout);
        }
        else
        {
            // 1st block
            CopyRangedProperties(oldLayout, 0, position, 0, newLayout);

            // Inserted text
            CopySinglePropertyRange(oldLayout, position - 1, newLayout, position, textToInsertLength, caretFormat);

            // Last block (if it exists)
            CopyRangedProperties(oldLayout, position, oldTextLength, textToInsertLength, newLayout);
        }

        // Copy trailing end.
        CopySinglePropertyRange(oldLayout, oldTextLength, newLayout, static_cast<UINT32>(text.length()), UINT32_MAX);
    }

	AngelGUISystem::GUIGlobalTools::SafeRelease(&oldLayout);

    return S_OK;
}


STDMETHODIMP EditableLayout::RemoveTextAt(
    IN OUT IDWriteTextLayout*& currentLayout,
    IN OUT std::wstring& text,
    UINT32 position,
    UINT32 lengthToRemove
    )
{
    // Removes text and shifts all formatting.

    HRESULT hr = S_OK;

    // copy all the old formatting.
    IDWriteTextLayout* oldLayout = AngelGUISystem::GUIGlobalTools::SafeAcquire(currentLayout);
    UINT32 oldTextLength = static_cast<UINT32>(text.length());

     // Remove the old text and recreate the new text layout.
    text.erase(position, lengthToRemove);
   

    if (SUCCEEDED(hr))
    {
        RecreateLayout(currentLayout, text);
    }

    IDWriteTextLayout* newLayout = currentLayout;

    if (SUCCEEDED(hr))
    {
        CopyGlobalProperties(oldLayout, newLayout);

        if (position == 0)
        {
            // The rest of the text
            CopyRangedProperties(oldLayout, lengthToRemove, oldTextLength, lengthToRemove, newLayout, true); 
        }
        else
        {
            // 1st block
            CopyRangedProperties(oldLayout, 0, position, 0, newLayout, true );

            // Last block (if it exists, we increment past the deleted text)
            CopyRangedProperties(oldLayout, position + lengthToRemove, oldTextLength, lengthToRemove, newLayout, true);
        }
        CopySinglePropertyRange(oldLayout, oldTextLength, newLayout, static_cast<UINT32>(text.length()), UINT32_MAX);
    }

	AngelGUISystem::GUIGlobalTools::SafeRelease(&oldLayout);

    return S_OK;
}


STDMETHODIMP EditableLayout::Clear(
    IN OUT IDWriteTextLayout*& currentLayout,
    IN OUT std::wstring& text
    )
{
    HRESULT hr = S_OK;

    try
    {
        text.clear();
    }
    catch (...)
    {
        HR(hr);
    }

    if (SUCCEEDED(hr))
    {
        hr = RecreateLayout(currentLayout, text);
    }

    return hr;
}


void EditableLayout::CopyGlobalProperties(
    IDWriteTextLayout* oldLayout,
    IDWriteTextLayout* newLayout
    )
{
    // Copies global properties that are not range based.

    newLayout->SetTextAlignment(oldLayout->GetTextAlignment());
    newLayout->SetParagraphAlignment(oldLayout->GetParagraphAlignment());
    newLayout->SetWordWrapping(oldLayout->GetWordWrapping());
    newLayout->SetReadingDirection(oldLayout->GetReadingDirection());
    newLayout->SetFlowDirection(oldLayout->GetFlowDirection());
    newLayout->SetIncrementalTabStop(oldLayout->GetIncrementalTabStop());

    DWRITE_TRIMMING trimmingOptions = {};
    IDWriteInlineObject* inlineObject = NULL;
    oldLayout->GetTrimming(&trimmingOptions, &inlineObject);
    newLayout->SetTrimming(&trimmingOptions, inlineObject);
	AngelGUISystem::GUIGlobalTools::SafeRelease(&inlineObject);

    DWRITE_LINE_SPACING_METHOD lineSpacingMethod = DWRITE_LINE_SPACING_METHOD_DEFAULT;
    FLOAT lineSpacing = 0;
    FLOAT baseline = 0;
    oldLayout->GetLineSpacing(&lineSpacingMethod, &lineSpacing, &baseline);
    newLayout->SetLineSpacing(lineSpacingMethod, lineSpacing, baseline);
}
