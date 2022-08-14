/**
    ===============================================================
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any internal version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <https://www.gnu.org/licenses/>.
    
    Copyright (c) 2022 ElandaSunshine
    ===============================================================
    
    @author Elanda
    @file   jaut_format_layout.h
    @date   12, September 2019
    
    ===============================================================
 */

#pragma once

namespace jaut
{
class FormatAttributes;

class FormatLayout final
{
private:
    template <typename Iterator>
    class DereferencingIterator
    {
    public:
        using value_type        = typename std::remove_reference<decltype(**std::declval<Iterator>())>::type;
        using difference_type   = typename std::iterator_traits<Iterator>::difference_type;
        using pointer           = value_type*;
        using reference         = value_type&;
        using iterator_category = typename std::iterator_traits<Iterator>::iterator_category;

        explicit DereferencingIterator (Iterator in) : iterator (std::move (in)) {}

        DereferencingIterator& operator+= (difference_type distance)
        {
            iterator += distance;
            return *this;
        }

        friend DereferencingIterator operator+(DereferencingIterator i, difference_type d) { return i += d; }
        friend DereferencingIterator operator+(difference_type d, DereferencingIterator i) { return i += d; }

        DereferencingIterator& operator-=(difference_type distance)
        {
            iterator -= distance;
            return *this;
        }

        friend DereferencingIterator operator- (DereferencingIterator i, difference_type d) { return i -= d; }

        friend difference_type operator-(DereferencingIterator a,
                                         DereferencingIterator b) { return a.iterator - b.iterator; }

        reference operator[] (difference_type d) const { return *iterator[d]; }

        friend bool operator<  (DereferencingIterator a, DereferencingIterator b) { return a.iterator <  b.iterator; }
        friend bool operator<= (DereferencingIterator a, DereferencingIterator b) { return a.iterator <= b.iterator; }
        friend bool operator>  (DereferencingIterator a, DereferencingIterator b) { return a.iterator >  b.iterator; }
        friend bool operator>= (DereferencingIterator a, DereferencingIterator b) { return a.iterator >= b.iterator; }
        friend bool operator== (DereferencingIterator a, DereferencingIterator b) { return a.iterator == b.iterator; }
        friend bool operator!= (DereferencingIterator a, DereferencingIterator b) { return a.iterator != b.iterator; }

        DereferencingIterator& operator++()           { ++iterator; return *this; }
        DereferencingIterator& operator--()           { --iterator; return *this; }
        DereferencingIterator  operator++ (int) const { DereferencingIterator copy (*this); ++(*this); return copy; }
        DereferencingIterator  operator-- (int) const { DereferencingIterator copy (*this); --(*this); return copy; }

        reference operator* () const { return **iterator; }
        pointer   operator->() const { return  *iterator; }

    private:
        Iterator iterator;
    };

public:
    FormatLayout();
    FormatLayout (const FormatLayout&);
    FormatLayout& operator= (const FormatLayout&);
    FormatLayout (FormatLayout&&) noexcept;
    FormatLayout& operator= (FormatLayout&&) noexcept;

    ~FormatLayout();

    //==============================================================================
    void createLayout (const FormatAttributes&, float maxWidth);
    void createLayout (const FormatAttributes&, float maxWidth, float maxHeight);
    void createLayoutWithBalancedLineLengths (const FormatAttributes&, float maxWidth);
    void createLayoutWithBalancedLineLengths (const FormatAttributes&, float maxWidth, float maxHeight);
    void draw (juce::Graphics&, juce::Rectangle<float> area) const;

    //==============================================================================
    class JUCE_API  Glyph
    {
    public:
        Glyph (int glyphCode, juce::Point<float> anchor, float width) noexcept;
        Glyph (const Glyph&) noexcept;
        Glyph& operator= (const Glyph&) noexcept;
        ~Glyph() noexcept;

        int glyphCode;
        juce::Point<float> anchor;
        float width;

    private:
        JUCE_LEAK_DETECTOR (Glyph)
    };

    //==============================================================================
    class JUCE_API Run
    {
    public:
        Run() noexcept;
        Run(const Run&);
        Run(juce::Range<int> stringRange, int numGlyphsToPreallocate, bool isUnderlined);
        ~Run() noexcept;
    
        juce::Range<float> getRunBoundsX() const noexcept;
    
        juce::Font font;
        juce::Colour colour;
        juce::Array<Glyph> glyphs;
        juce::Range<int> stringRange;
        bool isUnderlined;

    private:
        Run& operator= (const Run&);
        JUCE_LEAK_DETECTOR (Run)
    };

    //==============================================================================
    class JUCE_API  Line
    {
    public:
        Line() noexcept;
        Line (const Line&);
        Line (juce::Range<int> stringRange, juce::Point<float> lineOrigin,
              float ascent, float descent, float leading, int numRunsToPreallocate);
        ~Line() noexcept;
    
        juce::Range<float> getLineBoundsX() const noexcept;
        juce::Range<float> getLineBoundsY() const noexcept;
        juce::Rectangle<float> getLineBounds() const noexcept;
    
        juce::OwnedArray<Run> runs;
        juce::Range<int> stringRange;
        juce::Point<float> lineOrigin;
        float ascent, descent, leading;

    private:
        Line& operator= (const Line&);
        JUCE_LEAK_DETECTOR (Line)
    };

    //==============================================================================
    float getWidth() const noexcept     { return width; }
    float getHeight() const noexcept    { return height; }
    int getNumLines() const noexcept    { return lines.size(); }
    Line& getLine (int index) const noexcept;
    void addLine (std::unique_ptr<Line>);
    void ensureStorageAllocated (int numLinesNeeded);

    using       iterator = DereferencingIterator<      Line* const*>;
    using const_iterator = DereferencingIterator<const Line* const*>;

          iterator  begin()       { return       iterator (lines.begin()); }
    const_iterator  begin() const { return const_iterator (lines.begin()); }
    const_iterator cbegin() const { return const_iterator (lines.begin()); }
          iterator  end()       { return       iterator (lines.end()); }
    const_iterator  end() const { return const_iterator (lines.end()); }
    const_iterator cend() const { return const_iterator (lines.end()); }
    void recalculateSize();

private:
    juce::OwnedArray<Line> lines;
    float width, height;
    juce::Justification justification;

    void createStandardLayout (const FormatAttributes&);
    bool createNativeLayout (const FormatAttributes&);

    JUCE_LEAK_DETECTOR (FormatLayout)
};
}
