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
    @file   jaut_SizeStructs.h
    @date   20, July 2020
    
    ===============================================================
 */

#pragma once

#include <jaut_core/define/jaut_Define.h>

#include <juce_graphics/juce_graphics.h>



//**********************************************************************************************************************
// region Header
//======================================================================================================================
namespace jaut
{
    //==================================================================================================================
    /**
     *  A simple extension to juce::Rectangle that can be used for algorithmic padding, margin as it is the case
     *  in CSS environments.
     *  
     *  @tparam T The floating point or integral type that represents the size
     */
    template<class T>
    struct JAUT_API Distance
    {
        //==============================================================================================================
        static_assert(std::is_floating_point_v<T> || std::is_integral_v<T>,
                      "Type T must be either a floating point or integral type");
        
        //==============================================================================================================
        T left   {};
        T top    {};
        T right  {};
        T bottom {};
        
        //==============================================================================================================
        /** Creates a new instance with all values 0. */
        constexpr Distance() noexcept = default;
        
        /**
         *  Creates a new instance with the specified values.
         *  
         *  @param leftDistance   The distance from the left in pixel
         *  @param topDistance    The distance from the top in pixel
         *  @param rightDistance  The distance from the right in pixel
         *  @param bottomDistance The distance from the bottom in pixel
         */
        constexpr Distance(T leftDistance, T topDistance, T rightDistance, T bottomDistance) noexcept;
        
        /**
         *  Creates a new instance with the specified values.
         *  
         *  @param topDistance          The distance from the top in pixel
         *  @param leftAndRightDistance The distance from the right and the left in pixel
         *  @param bottomDistance       The distance from the bottom in pixel
         */
        constexpr Distance(T topDistance, T leftAndRightDistance, T bottomDistance) noexcept;
        
        /**
         *  Creates a new instance with the specified values.
         *  
         *  @param leftAndRightDistance The distance from the right and the left in pixel
         *  @param topAndBottomDistance The distance from the top and the bottom in pixel
         */
        constexpr Distance(T leftAndRightDistance, T topAndBottomDistance) noexcept;
        
        /**
         *  Creates a new instance with the specified values.
         *  
         *  @param allSides The distance from all sides in pixel
         */
        constexpr Distance(T allSides) noexcept; // NOLINT
        
        //==============================================================================================================
        JAUT_NODISCARD
        constexpr bool operator==(const Distance<T> &other) const noexcept;
        
        JAUT_NODISCARD
        constexpr bool operator!=(const Distance<T> &other) const noexcept;
        
        //==============================================================================================================
        /**
         *  Whether the distance equals zero on all ends.
         *  
         *  @return True if all sides are zero
         */
        JAUT_NODISCARD
        constexpr bool isEmpty() const noexcept;
        
        //==============================================================================================================
        /**
         *  Converts this distance object to the same but as double.
         *  
         *  @return The new double distance object
         */
        JAUT_NODISCARD
        constexpr Distance<double> toDouble() const noexcept;
        
        /**
         *  Converts this distance object to the same but as int.
         *  
         *  @return The new int distance object
         */
        JAUT_NODISCARD
        constexpr Distance<int> toInt() const noexcept;
        
        //==============================================================================================================
        /**
         *  Returns a new object with this' data and a new left.
         *  
         *  @return The new version
         */
        JAUT_NODISCARD
        constexpr Distance<T> withLeft(T newLeft) const noexcept;
        
        /**
         *  Returns a new object with this' data and a new top.
         *  
         *  @return The new version
         */
        JAUT_NODISCARD
        constexpr Distance<T> withTop(T newTop) const noexcept;
        
        /**
         *  Returns a new object with this' data and a new right.
         *  
         *  @return The new version
         */
        JAUT_NODISCARD
        constexpr Distance<T> withRight(T newRight) const noexcept;
        
        /**
         *  Returns a new object with this' data and a new bottom.
         *  
         *  @return The new version
         */
        JAUT_NODISCARD
        constexpr Distance<T> withBottom(T newBottom) const noexcept;
        
        //==============================================================================================================
        /**
         *  Trims a juce rectangle on all sides with the specified values in this distance object.
         *  
         *  @param rect The rectangle to trim
         *  
         *  @return The trimmed rectangle object
         */
        JAUT_NODISCARD
        juce::Rectangle<T> trimRectangle(juce::Rectangle<T> rect) const noexcept;
    };
    
    //==================================================================================================================
    /** Specifies the thickness of a border or outline. */
    template<class T>
    using Thickness = Distance<T>;
    
    /** Specifies the distance from the outside of a component to the border of its parent. */
    template<class T>
    using Margin = Distance<T>;
    
    /** Specifies the distance from the inside of a component to all its children. */
    template<class T>
    using Padding = Distance<T>;
    
    //==================================================================================================================
    /**
     *  A convenience wrapper around two numeric types, that represent a size of some sorts.
     *  
     *  @tparam T The floating point or integral type that represents the size
     */
    template<class T>
    struct JAUT_API Size
    {
        //==============================================================================================================
        static_assert(std::is_floating_point_v<T> || std::is_integral_v<T>,
                      "Type T must be either a floating point or integral type");
        
        //==============================================================================================================
        T width  {};
        T height {};
        
        //==============================================================================================================
        constexpr Size(T width, T height) noexcept;
        constexpr Size(T widthAndHeight) noexcept; // NOLINT
    
        template<class U>
        explicit Size(const juce::Rectangle<U> &rect) noexcept;
        
        /** Constructs a new size object with size 0. */
        constexpr Size() noexcept = default;
        
        //==============================================================================================================
        JAUT_NODISCARD
        constexpr bool operator==(const Size<T> &other) const noexcept;
        
        JAUT_NODISCARD
        constexpr bool operator!=(const Size<T> &other) const noexcept;
        
        //==============================================================================================================
        /**
         *  Whether the size equals zero on both ends.
         *  
         *  @return True if width and height are zero
         */
        JAUT_NODISCARD
        constexpr bool isEmpty() const noexcept;
        
        //==============================================================================================================
        /**
         *  Expand the size with the given width and height.
         *  
         *  @param width  The additional width
         *  @param height The additional height
         */
        void expand(T width, T height) noexcept;
        
        /**
         *  Reduce the size by the given width and height.
         *  
         *  @param width  The width to reduce
         *  @param height The height to reduce
         */
        void reduce(T width, T height) noexcept;
        
        /**
         *  Expand the size with the given width and height and return the expanded struct.
         *  
         *  @param width  The additional width
         *  @param height The additional height
         *  
         *  @return The expanded Size struct
         */
        JAUT_NODISCARD
        constexpr Size<T> expanded(T width, T height) const noexcept;
        
        /**
         *  Reduce the size by the given width and height and return the reduced struct.
         *  
         *  @param width  The width to reduce
         *  @param height The height to reduce
         *  
         *  @return The reduced Size object
         */
        JAUT_NODISCARD
        constexpr Size<T> reduced(T width, T height) const noexcept;
        
        //==============================================================================================================
        /**
         *  Converts this distance object to the same but as double.
         *  
         *  @return The new double distance object
         */
        JAUT_NODISCARD
        constexpr Size<double> toDouble() const noexcept;
        
        /**
         *  Converts this distance object to the same but as int.
         *  
         *  @return The new int distance object
         */
        JAUT_NODISCARD
        constexpr Size<int> toInt() const noexcept;
    };
    
    //==================================================================================================================
    struct JAUT_API DistanceUtil
    {
        //==============================================================================================================
        /**
         *  Creates a juce::Path object from the given thickness object and the area of application.<br>
         *  The resulting shape is a rectangle as given by "area" with no fill,
         *  that will have its borders the thickness given for each side.
         *  
         *  @param area      The area to apply this border to
         *  @param thickness The thickness object
         *  
         *  @return The new path object
         */
        template<class T>
        JAUT_NODISCARD
        static juce::Path borderPathFromThickness(const juce::Rectangle<T> &area, Thickness<T> thickness);
    };
}
//======================================================================================================================
// endregion Header
//**********************************************************************************************************************
// region Implementation (Distance)
//======================================================================================================================
namespace jaut
{
    //==================================================================================================================
    template<class T>
    inline constexpr Distance<T>::Distance(T parLeftDistance,
                                           T parTopDistance,
                                           T parRightDistance,
                                           T parBottomDistance) noexcept
        : left  (parLeftDistance),
          top   (parTopDistance),
          right (parRightDistance),
          bottom(parBottomDistance)
    {}
    
    template<class T>
    inline constexpr Distance<T>::Distance(T parTopDistance, T parLeftAndRightDistance, T parBottomDistance) noexcept
        : Distance(parLeftAndRightDistance, parTopDistance, parLeftAndRightDistance, parBottomDistance)
    {}
    
    template<class T>
    inline constexpr Distance<T>::Distance(T parLeftAndRightDistance, T parTopAndBottomDistance) noexcept
        : Distance(parTopAndBottomDistance, parLeftAndRightDistance, parTopAndBottomDistance)
    {}
    
    template<class T>
    inline constexpr Distance<T>::Distance(T parAllSides) noexcept
        : Distance(parAllSides, parAllSides)
    {}
    
    //==================================================================================================================
    template<class T>
    inline constexpr bool Distance<T>::operator==(const Distance<T> &other) const noexcept
    {
        return (left == other.left && top == other.top && right == other.right && bottom == other.bottom);
    }
    
    template<class T>
    inline constexpr bool Distance<T>::operator!=(const Distance<T> &other) const noexcept
    {
        return !(operator==(other));
    }
    
    //==================================================================================================================
    template<class T>
    inline constexpr bool Distance<T>::isEmpty() const noexcept
    {
        return (left == 0 && top == 0 && right == 0 && bottom == 0);
    }
    
    //==================================================================================================================
    template<class T>
    inline constexpr Distance<double> Distance<T>::toDouble() const noexcept
    {
        if constexpr (std::is_same_v<T, double>)
        {
            return *this;
        }
        else
        {
            return {
                static_cast<double>(left),
                static_cast<double>(top),
                static_cast<double>(right),
                static_cast<double>(bottom)
            };
        }
    }
    
    template<class T>
    inline constexpr Distance<int> Distance<T>::toInt() const noexcept
    {
        if constexpr (std::is_same_v<T, int>)
        {
            return *this;
        }
        else
        {
            return {
                static_cast<int>(left),
                static_cast<int>(top),
                static_cast<int>(right),
                static_cast<int>(bottom)
            };
        }
    }
    
    //==================================================================================================================
    template<class T>
    inline constexpr Distance<T> Distance<T>::withLeft(T parNewLeft) const noexcept
    {
        Distance<T> temp = *this;
        temp.left = parNewLeft;
        return temp;
    }
    
    template<class T>
    inline constexpr Distance<T> Distance<T>::withTop(T parNewTop) const noexcept
    {
        Distance<T> temp = *this;
        temp.top = parNewTop;
        return temp;
    }
    
    template<class T>
    inline constexpr Distance<T> Distance<T>::withRight(T parNewRight) const noexcept
    {
        Distance<T> temp = *this;
        temp.right = parNewRight;
        return temp;
    }
    
    template<class T>
    inline constexpr Distance<T> Distance<T>::withBottom(T parNewBottom) const noexcept
    {
        Distance<T> temp = *this;
        temp.bottom = parNewBottom;
        return temp;
    }
    
    //==================================================================================================================
    template<class T>
    inline juce::Rectangle<T> Distance<T>::trimRectangle(juce::Rectangle<T> rect) const noexcept
    {
        return rect.withTrimmedLeft(left)
                   .withTrimmedTop(top)
                   .withTrimmedRight(right)
                   .withTrimmedBottom(bottom);
    }
}
//======================================================================================================================
// endregion Implementation (Distance)
//**********************************************************************************************************************
// region Implementation (Size)
//======================================================================================================================
namespace jaut
{
    //==================================================================================================================
    template<class T>
    inline constexpr Size<T>::Size(T parWidth, T parHeight) noexcept
        : width (parWidth),
          height(parHeight)
    {}
    
    template<class T>
    inline constexpr Size<T>::Size(T widthAndHeight) noexcept
        : Size(widthAndHeight, widthAndHeight)
    {}
    
    template<class T>
    template<class U>
    inline Size<T>::Size(const juce::Rectangle<U> &rect) noexcept
        : Size(static_cast<T>(rect.getWidth()), static_cast<T>(rect.getHeight()))
    {}
    
    //==================================================================================================================
    template<class T>
    inline constexpr bool Size<T>::operator==(const Size<T> &other) const noexcept
    {
        return (width == other.width && height == other.height);
    }
    
    template<class T>
    inline constexpr bool Size<T>::operator!=(const Size<T> &other) const noexcept
    {
        return !(operator==(other));
    }
    
    //==================================================================================================================
    template<class T>
    inline constexpr bool Size<T>::isEmpty() const noexcept
    {
        return (width == 0 && height == 0);
    }
    
    //==================================================================================================================
    template<class T>
    inline void Size<T>::expand(T parWidth, T parHeight) noexcept
    {
        width  += parWidth;
        height += parHeight;
    }
    
    template<class T>
    inline void Size<T>::reduce(T parWidth, T parHeight) noexcept
    {
        width  -= parWidth;
        height -= parHeight;
    }
    
    template<class T>
    inline constexpr Size<T> Size<T>::expanded(T parWidth, T parHeight) const noexcept
    {
        return { width + parWidth, height + parHeight };
    }
    
    template<class T>
    inline constexpr Size<T> Size<T>::reduced(T parWidth, T parHeight) const noexcept
    {
        return { width - parWidth, height - parHeight };
    }
    
    //==================================================================================================================
    template<class T>
    inline constexpr Size<double> Size<T>::toDouble() const noexcept
    {
        if constexpr (std::is_same_v<T, double>)
        {
            return *this;
        }
        else
        {
            return {
                static_cast<double>(width),
                static_cast<double>(height)
            };
        }
    }
    
    template<class T>
    inline constexpr Size<int> Size<T>::toInt() const noexcept
    {
        if constexpr (std::is_same_v<T, int>)
        {
            return *this;
        }
        else
        {
            return {
                static_cast<int>(width),
                static_cast<int>(height)
            };
        }
    }
}
//======================================================================================================================
// endregion Implementation (Size)
//**********************************************************************************************************************
// region Implementation (DistanceUtil)
//======================================================================================================================
namespace jaut
{
    //==================================================================================================================
    template<class T>
    inline juce::Path DistanceUtil::borderPathFromThickness(const juce::Rectangle<T> &area, Thickness<T> thickness)
    {
        juce::Path path;
        
        path.addRectangle(area.withWidth (thickness.left));
        path.addRectangle(area.withHeight(thickness.top));
        path.addRectangle(area.withWidth(thickness.right)  .withX(area.getX() + area.getWidth()  - thickness.right));
        path.addRectangle(area.withHeight(thickness.bottom).withY(area.getY() + area.getHeight() - thickness.bottom));
        
        return path;
    }
}
//======================================================================================================================
// endregion Implementation (DistanceUtil)
//**********************************************************************************************************************
