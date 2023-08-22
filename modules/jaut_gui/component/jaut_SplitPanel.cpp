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
    @file   jaut_SplitPane.cpp
    @date   20, July 2020
    
    ===============================================================
 */

#include <jaut_gui/component/jaut_SplitPanel.h>

#include <jaut_gui/lookandfeel/jaut_LookAndFeel.h>



//**********************************************************************************************************************
// region Namespace
//======================================================================================================================
namespace
{}
//======================================================================================================================
// endregion Namespace
//**********************************************************************************************************************
// region SplitPane::Pimpl
//======================================================================================================================
namespace jaut
{
    //==================================================================================================================
    class SplitPanel::Pimpl : private juce::MouseListener
    {
    public:
        enum class CollapseState
        {
            None,
            Pane1,
            Pane2,
            All
        };
        
        //==============================================================================================================
        class LockedAxisConstrainer : public juce::ComponentBoundsConstrainer
        {
        public:
            SplitPanel &panel;
            
            //==========================================================================================================
            explicit LockedAxisConstrainer(SplitPanel &parPanel) noexcept
                : panel(parPanel)
            {}
            
            //==========================================================================================================
            void applyBoundsToComponent(Component &parComponent, juce::Rectangle<int> parBounds) override
            {
                const juce::Rectangle content_bounds = panel.getContentBounds();
                const int             thickness      = panel.getSeparatorThickness();
                
                if (panel.getOrientation() == Orientation::Vertical)
                {
                    const int min_pos = content_bounds.getX() + panel.getMinSizePane1().width;
                    const int max_pos = std::max(min_pos, content_bounds.getRight()
                                                            - panel.getMinSizePane2().width
                                                            - thickness);
                    
                    parComponent.setBounds(parBounds.withX(std::clamp(parBounds.getX(), min_pos, max_pos))
                                                    .withY(content_bounds.getY()));
                }
                else
                {
                    const int min_pos = content_bounds.getY() + panel.getMinSizePane1().height;
                    const int max_pos = std::max(min_pos, content_bounds.getBottom()
                                                            - panel.getMinSizePane2().height
                                                            - thickness);
                    
                    parComponent.setBounds(parBounds.withY(std::clamp(parBounds.getY(), min_pos, max_pos))
                                                    .withX(content_bounds.getX()));
                }
            }
        };
        
        struct OrientedBounds
        {
            //==========================================================================================================
            struct Data
            {
                int contentStart {};
                int contentEnd {};
    
                int prevContentStart {};
                int prevContentEnd {};
    
                int size {};
                int prevSize {};
    
                int minP1 {};
                int minP2 {};
                
                juce::MouseCursor::StandardCursorType cursorType {};
            };
            
            //==========================================================================================================
            static OrientedBounds create(const Pimpl &parPimpl) noexcept
            {
                Data dv {
                    parPimpl.bounds    .getX(),
                    parPimpl.bounds    .getRight(),
                    parPimpl.prevBounds.getX(),
                    parPimpl.prevBounds.getRight(),
                    parPimpl.bounds    .getWidth(),
                    parPimpl.prevBounds.getWidth(),
                    parPimpl.panel.getMinSizePane1().width,
                    parPimpl.panel.getMinSizePane2().width,
                    juce::MouseCursor::LeftRightResizeCursor
                };
                
                Data dh {
                    parPimpl.bounds    .getY(),
                    parPimpl.bounds    .getBottom(),
                    parPimpl.prevBounds.getY(),
                    parPimpl.prevBounds.getBottom(),
                    parPimpl.bounds    .getHeight(),
                    parPimpl.prevBounds.getHeight(),
                    parPimpl.panel.getMinSizePane1().height,
                    parPimpl.panel.getMinSizePane2().height,
                    juce::MouseCursor::UpDownResizeCursor
                };
                
                return (parPimpl.panel.getOrientation() == Orientation::Vertical
                            ? OrientedBounds{ dv, dh }
                            : OrientedBounds{ dh, dv });
            }
            
            //==========================================================================================================
            Data current;
            Data other;
        };
        
        //==============================================================================================================
        using ComponentPtr = juce::OptionalScopedPointer<juce::Component>;
        
        //==============================================================================================================
        explicit Pimpl(SplitPanel &parPanel)
            : panel        (parPanel),
              constrainer  (parPanel),
              collapseState(getCollapseState(parPanel))
        {
            ContentPane &pane1 = getPane(PaneId::Pane1);
            ContentPane &pane2 = getPane(PaneId::Pane2);
            
            pane1.setVisible(!panel.getIsPane1Collapsed());
            DBG("TEST " << static_cast<int>(panel.getIsPane1Collapsed()) << "\n");
            panel.addChildComponent(pane1);
            
            pane2.setVisible(!panel.getIsPane2Collapsed());
            panel.addChildComponent(pane2);
            
            separator.addMouseListener(this, false);
            panel.addAndMakeVisible(separator);
            
            pane1.eventContentChanged += jaut::makeHandler([&panel = panel, &pane = panes[0]](juce::Component*)
            {
                panel.eventPaneChanged.invoke(pane, PaneId::Pane1);
            });
            
            pane2.eventContentChanged += jaut::makeHandler([&panel = panel, &pane = panes[1]](juce::Component*)
            {
                panel.eventPaneChanged.invoke(pane, PaneId::Pane2);
            });
        }
        
        //==============================================================================================================
        void setBounds(juce::Rectangle<int> parBounds) noexcept
        {
            const bool was_moved   = (prevBounds.getPosition() != parBounds.getPosition());
            const bool was_resized = (prevBounds.getWidth()  != parBounds.getWidth()
                                          || prevBounds.getHeight() != parBounds.getHeight());
            
            if (!was_moved && !was_resized)
            {
                return;
            }
            
            bounds   = parBounds;
            orBounds = OrientedBounds::create(*this);
            
            resized(was_moved, was_resized);
            
            prevBounds = parBounds;
            orBounds = OrientedBounds::create(*this);
        }
        
        //==============================================================================================================
        void resized(bool parWasMoved, bool parWasResized)
        {
            if (bounds.isEmpty())
            {
                return;
            }
            
            if (!initialised)
            {
                initSeparatorPos();
                initialised = true;
            }
            else if (parWasMoved || parWasResized)
            {
                updateSeparatorPos();
            }
        }
        
        void resizePanes()
        {
            const Size min_p1 = panel.getMinSizePane1();
            const Size min_p2 = panel.getMinSizePane2();
            
            ContentPane &pane1 = getPane(PaneId::Pane1);
            ContentPane &pane2 = getPane(PaneId::Pane2);
            
            switch (collapseState)
            {
                case CollapseState::None:
                {
                    if (panel.getOrientation() == Orientation::Vertical)
                    {
                        pane1.setBounds(bounds.withRight (separator.getX())
                                              .withHeight(std::max(bounds.getHeight(), min_p1.height)));
                        pane2.setBounds(bounds.withLeft (separator.getRight())
                                              .withWidth(std::max(
                                                  min_p2.width,
                                                  (bounds.getRight() - separator.getRight())
                                              ))
                                              .withHeight(std::max(bounds.getHeight(), min_p2.height)));
                    }
                    else
                    {
                        pane1.setBounds(bounds.withBottom(separator.getY())
                                              .withWidth (std::max(bounds.getWidth(), min_p1.width)));
                        pane2.setBounds(bounds.withTop   (separator.getBottom())
                                              .withHeight(std::max(
                                                  min_p2.height,
                                                  (bounds.getBottom() - separator.getBottom())
                                              ))
                                              .withWidth (std::max(bounds.getWidth(), min_p2.width)));
                    }
    
                    break;
                }
    
                case CollapseState::Pane1:
                {
                    pane2.setBounds(bounds.withWidth (std::max(min_p2.width,  bounds.getWidth()))
                                          .withHeight(std::max(min_p2.height, bounds.getHeight())));
                    break;
                }
    
                case CollapseState::Pane2:
                {
                    pane1.setBounds(bounds.withWidth (std::max(min_p1.width,  bounds.getWidth()))
                                          .withHeight(std::max(min_p1.height, bounds.getHeight())));
                    break;
                }
                
                default:;
            }
        }
        
        //==============================================================================================================
        void paintSeparator(juce::Graphics &parG) const
        {
            if (!separator.isVisible())
            {
                return;
            }
            
            panel.lookAndFeel->drawSplitPanelSeparator(parG, separator);
        }
        
        //==============================================================================================================
        void componentChanged(const juce::Component *const parComp)
        {
            if (parComp != &separator)
            {
                return;
            }
            
            resizePanes();
        }
        
        //==============================================================================================================
        JAUT_NODISCARD
        ContentPane& getPane(PaneId parPaneType) noexcept
        {
            return panes[static_cast<std::size_t>(parPaneType) - 1];
        }
        
        //==============================================================================================================
        void updateOrientation(bool parUpdateSeparator)
        {
            orBounds = OrientedBounds::create(*this);
            separator.setMouseCursor(orBounds.current.cursorType);
            
            if (parUpdateSeparator)
            {
                const double sep_rel     = (actualSeparatorPos - orBounds.current.contentStart);
                const double sep_factor  = (1.0 / orBounds.other.size) * sep_rel;
                const double sep_new_abs = orBounds.current.contentStart + (orBounds.current.size * sep_factor);
                
                const double old_separator_pos = actualSeparatorPos;
                
                actualSeparatorPos = clampToMin(sep_new_abs, orBounds);
                (void) applySeparatorBounds();
                
                postSeparatorMoved(old_separator_pos);
            }
        }
        
        void updateCollapsedPanes()
        {
            ContentPane &pane1 = getPane(PaneId::Pane1);
            ContentPane &pane2 = getPane(PaneId::Pane2);
            
            const CollapseState prev_cstate = std::exchange(collapseState, getCollapseState(panel));
            
            pane1.setVisible(!panel.getIsPane1Collapsed());
            pane2.setVisible(!panel.getIsPane2Collapsed());
            
            separator.setVisible(collapseState == CollapseState::None);
            
            if (prev_cstate != collapseState && collapseState == CollapseState::None)
            {
                postSeparatorMoved(0.0);
            }
            
            resizePanes();
        }
        
        void updateSeparatorPos()
        {
            const double old_separator_pos = actualSeparatorPos;
            actualSeparatorPos = calculateSeparatorPos(orBounds);
            
            if (!applySeparatorBounds())
            {
                resizePanes();
            }
            
            postSeparatorMoved(old_separator_pos);
        }
        
        //==============================================================================================================
        bool applySeparatorBounds()
        {
            if (collapseState != CollapseState::None)
            {
                return false;
            }
            
            const juce::Rectangle old_separator_bounds = separator.getBounds();
            
            if (panel.getOrientation() == Orientation::Vertical)
            {
                separator.setBounds(bounds.withWidth(panel.getSeparatorThickness())
                                          .withX(static_cast<int>(std::round(actualSeparatorPos))));
            }
            else
            {
                separator.setBounds(bounds.withHeight(panel.getSeparatorThickness())
                                          .withY(static_cast<int>(std::round(actualSeparatorPos))));
            }
            
            return (old_separator_bounds != separator.getBounds());
        }
        
        //==============================================================================================================
        JAUT_NODISCARD
        double getSeparatorPos() const noexcept
        {
            return actualSeparatorPos - orBounds.current.contentStart;
        }
        
        //==============================================================================================================
        void setSeparatorPos(int parNewPos)
        {
            const double old_separator_pos = actualSeparatorPos;
            actualSeparatorPos = clampToMin(orBounds.current.contentStart + parNewPos, orBounds);
            
            if (!applySeparatorBounds())
            {
                resizePanes();
            }
    
            postSeparatorMoved(old_separator_pos);
        }
        
        void moveSeparator(int parAmount)
        {
            const double old_separator_pos = actualSeparatorPos;
            actualSeparatorPos = clampToMin(actualSeparatorPos + parAmount, orBounds);
            
            if (!applySeparatorBounds())
            {
                resizePanes();
            }
    
            postSeparatorMoved(old_separator_pos);
        }
        
        //==============================================================================================================
        void updateMins()
        {
            const Size min_p1 = panel.getMinSizePane1();
            const Size min_p2 = panel.getMinSizePane2();
            
            if (panel.getOrientation() == Orientation::Vertical)
            {
                orBounds.current.minP1 = min_p1.width;
                orBounds.current.minP2 = min_p2.width;
            }
            else
            {
                orBounds.current.minP1 = min_p1.height;
                orBounds.current.minP2 = min_p2.height;
            }
            
            updateSeparatorPos();
        }
        
    private:
        JAUT_NODISCARD
        static CollapseState getCollapseState(const SplitPanel &panel) noexcept
        {
            return static_cast<CollapseState>((panel.getIsPane1Collapsed() * 1) + (panel.getIsPane2Collapsed() * 2));
        }
        
        //==============================================================================================================
        SplitPanel &panel;
        
        std::array<ContentPane, 2> panes;
        juce::Component            separator;
        
        LockedAxisConstrainer  constrainer;
        juce::ComponentDragger dragger;
        
        OrientedBounds orBounds;
        
        juce::Rectangle<int> bounds;
        juce::Rectangle<int> prevBounds;
        
        double        actualSeparatorPos { 0.0 };
        CollapseState collapseState;
        
        bool initialised { false };
        
        //==============================================================================================================
        JAUT_NODISCARD
        double calculateSeparatorPos(const OrientedBounds &parOrBounds) const noexcept
        {
            const double rdist = panel.getResizeDistributionFactor();
            
            // The redistribution must be a value between 0.0 and 1.0
            jassert(rdist >= 0.0 && rdist <= 1.0);
            
            const int    diff   = parOrBounds.current.size - parOrBounds.current.prevSize;
            const double change = diff * rdist;
            
            return clampToMin(actualSeparatorPos + change, parOrBounds);
        }
        
        JAUT_NODISCARD
        double clampToMin(double parSeparatorPos, const OrientedBounds &parOrBounds) const noexcept
        {
            const int min_pos = parOrBounds.current.contentStart + parOrBounds.current.minP1;
            const int max_pos = parOrBounds.current.contentEnd
                                  - parOrBounds.current.minP2
                                  - panel.getSeparatorThickness();
            
            if (parSeparatorPos < min_pos)
            {
                return min_pos;
            }
            
            if (parSeparatorPos > max_pos)
            {
                return std::max(min_pos, max_pos);
            }
            
            return parSeparatorPos;
        }
        
        //==============================================================================================================
        void postSeparatorMoved(double parPrevValue)
        {
            if (separator.isVisible() && parPrevValue != actualSeparatorPos)
            {
                const int new_pos = static_cast<int>(std::round(actualSeparatorPos)) - orBounds.current.contentStart;
                const int old_pos = static_cast<int>(std::round(parPrevValue))       - orBounds.current.contentStart;
                panel.eventSeparatorMoved.invoke(new_pos, old_pos);
            }
        }
        
        //==============================================================================================================
        void initSeparatorPos()
        {
            const double old_separator_pos = actualSeparatorPos;
            
            const int centre   = orBounds.current.contentStart + static_cast<int>(orBounds.current.size / 2.0);
            actualSeparatorPos = clampToMin(centre, orBounds);
            
            if (!applySeparatorBounds())
            {
                resizePanes();
            }
    
            postSeparatorMoved(old_separator_pos);
        }
        
        //==============================================================================================================
        void mouseDown(const juce::MouseEvent &parE) override
        {
            dragger.startDraggingComponent(&separator, parE);
        }
        
        void mouseDrag(const juce::MouseEvent &parE) override
        {
            dragger.dragComponent(&separator, parE, &constrainer);
            
            const double old_separator_pos = actualSeparatorPos;
            actualSeparatorPos = (panel.getOrientation() == Orientation::Vertical
                                      ? separator.getX()
                                      : separator.getY());
            
            postSeparatorMoved(old_separator_pos);
        }
    };
}
//======================================================================================================================
// endregion SplitPane::Pimpl
//**********************************************************************************************************************
// region SplitPane
//======================================================================================================================
namespace jaut
{
    //==================================================================================================================
    SplitPanel::SplitPanel(const juce::String &parName,
                           InitStyle          parStyle,
                           Options            parOptions)
        : ExtraComponent(parName, static_cast<ContainerStyle>(parStyle)), // NOLINT
          style         (static_cast<Style>(parStyle)), // NOLINT
          options       (parOptions),
          pimpl         (std::make_unique<Pimpl>(*this))
    {
        JAUT_INTERN_INIT_LAF();
        
        pimpl->updateOrientation(false);
        pimpl->updateCollapsedPanes();
    }
    
    SplitPanel::SplitPanel(InitStyle parStyle, Options parOptions)
        : SplitPanel("", parStyle, parOptions)
    {}
    
    SplitPanel::SplitPanel(const juce::String &parName, InitStyle parStyle)
        : SplitPanel(parName, parStyle, {})
    {}
    
    SplitPanel::SplitPanel(InitStyle parStyle)
        : SplitPanel("", parStyle, {})
    {}
    
    SplitPanel::SplitPanel(const juce::String &parName, Options parOptions)
        : SplitPanel(parName, {}, parOptions)
    {}
    
    SplitPanel::SplitPanel(Options parOptions)
        : SplitPanel("", {}, parOptions)
    {}
    
    SplitPanel::SplitPanel(const juce::String &parName)
        : SplitPanel(parName, {}, {})
    {}
    
    SplitPanel::SplitPanel()
        : SplitPanel("", {}, {})
    {}
    
    SplitPanel::~SplitPanel() = default;
    
    //==================================================================================================================
    void SplitPanel::paintContents(juce::Graphics &parG)
    {
        pimpl->paintSeparator(parG);
    }
    
    void SplitPanel::resizeContents()
    {
        pimpl->setBounds(getContentBounds());
    }
    
    //==================================================================================================================
    int SplitPanel::getSeparatorPos() const noexcept
    {
        return static_cast<int>(std::round(pimpl->getSeparatorPos()));
    }
    
    double SplitPanel::getSeparatorPosNormalised() const noexcept
    {
        const int pos = static_cast<int>(std::round(pimpl->getSeparatorPos()));
        return (options.orientation == Orientation::Vertical
            ? ((1.0 / (getContentBounds().getWidth()  - getSeparatorThickness())) * pos)
            : ((1.0 / (getContentBounds().getHeight() - getSeparatorThickness())) * pos));
    }
    
    //==================================================================================================================
    void SplitPanel::setSeparatorPos(int parPos)
    {
        if (parPos >= 0)
        {
            pimpl->setSeparatorPos(parPos);
        }
    }
    
    void SplitPanel::setSeparatorPosNormalised(double parNormalisedPos)
    {
        if (getContentBounds().isEmpty())
        {
            return;
        }
        
        if (parNormalisedPos >= 0.0 && parNormalisedPos <= 1.0)
        {
            const int size = [](juce::Rectangle<int> bounds, Orientation orientation, int sep)
            {
                return (orientation == Orientation::Vertical
                    ? bounds.getWidth()
                    : bounds.getHeight()) - sep;
            } (getContentBounds(), getOrientation(), getSeparatorThickness());
            
            pimpl->setSeparatorPos(static_cast<int>(std::round(size * parNormalisedPos)));
        }
    }
    
    //==================================================================================================================
    void SplitPanel::moveSeparator(int parAmount)
    {
        if (parAmount != 0)
        {
            pimpl->moveSeparator(parAmount);
        }
    }
    
    void SplitPanel::moveSeparatorNormalised(double parNormalisedAmount)
    {
        if (getContentBounds().isEmpty())
        {
            return;
        }
        
        if (parNormalisedAmount > -1.0 && parNormalisedAmount <= 1.0)
        {
            const int size = [](juce::Rectangle<int> bounds, Orientation orientation, int sep)
            {
                return (orientation == Orientation::Vertical
                    ? bounds.getWidth()
                    : bounds.getHeight()) - sep;
            } (getContentBounds(), getOrientation(), getSeparatorThickness());
            
            pimpl->moveSeparator(static_cast<int>(std::round(size * parNormalisedAmount)));
        }
    }
    
    //==================================================================================================================
    ContentPane& SplitPanel::getPane1() noexcept
    {
        return pimpl->getPane(PaneId::Pane1);
    }
    
    const ContentPane& SplitPanel::getPane1() const noexcept
    {
        return pimpl->getPane(PaneId::Pane1);
    }
    
    ContentPane& SplitPanel::getPane2() noexcept
    {
        return pimpl->getPane(PaneId::Pane2);
    }
    
    const ContentPane& SplitPanel::getPane2() const noexcept
    {
        return pimpl->getPane(PaneId::Pane2);
    }
    
    //==================================================================================================================
    int SplitPanel::getSeparatorThickness() const noexcept
    {
        return style.separatorThickness;
    }
    
    //==================================================================================================================
    Size<int> SplitPanel::getMinSizePane1() const noexcept
    {
        return options.minSizePane1;
    }
    
    Size<int> SplitPanel::getMinSizePane2() const noexcept
    {
        return options.minSizePane2;
    }
    
    SplitPanel::Orientation SplitPanel::getOrientation() const noexcept
    {
        return options.orientation;
    }
    
    double SplitPanel::getResizeDistributionFactor() const noexcept
    {
        return options.resizeDistributionFactor;
    }
    
    bool SplitPanel::getIsPane1Collapsed() const noexcept
    {
        return options.collapsePane1;
    }
    
    bool SplitPanel::getIsPane2Collapsed() const noexcept
    {
        return options.collapsePane2;
    }
    
    //==================================================================================================================
    void SplitPanel::setSeparatorThickness(int parThickness)
    {
        std::swap(style.separatorThickness, parThickness);
        
        if (parThickness != style.separatorThickness)
        {
            pimpl->updateSeparatorPos();
        }
    }
    
    //==================================================================================================================
    void SplitPanel::setMinSizePane1(Size<int> parMinSize)
    {
        std::swap(options.minSizePane1, parMinSize);
        
        if (parMinSize != options.minSizePane1)
        {
            pimpl->updateMins();
        }
    }
    
    void SplitPanel::setMinSizePane2(Size<int> parMinSize)
    {
        std::swap(options.minSizePane2, parMinSize);
        
        if (parMinSize != options.minSizePane2)
        {
            pimpl->updateMins();
        }
    }
    
    void SplitPanel::setOrientation(Orientation parOrientation)
    {
        std::swap(options.orientation, parOrientation);
        
        if (parOrientation != options.orientation)
        {
            pimpl->updateOrientation(true);
        }
    }
    
    void SplitPanel::setResizeDistributionFactor(double parResizeDistributionFactor) noexcept
    {
        std::swap(options.resizeDistributionFactor, parResizeDistributionFactor);
    }
    
    void SplitPanel::setPane1Collapsed(bool parShouldCollapse)
    {
        std::swap(options.collapsePane1, parShouldCollapse);
        
        if (parShouldCollapse != options.collapsePane1)
        {
            pimpl->updateCollapsedPanes();
        }
    }
    
    void SplitPanel::setPane2Collapsed(bool parShouldCollapse)
    {
        std::swap(options.collapsePane2, parShouldCollapse);
        
        if (parShouldCollapse != options.collapsePane2)
        {
            pimpl->updateCollapsedPanes();
        }
    }
    
    //==================================================================================================================
    const SplitPanel::Style& SplitPanel::getStyle() const noexcept
    {
        return style;
    }
    
    const SplitPanel::Options& SplitPanel::getOptions() const noexcept
    {
        return options;
    }
    
    //==================================================================================================================
    void SplitPanel::childBoundsChanged(juce::Component *const parChild)
    {
        pimpl->componentChanged(parChild);
    }
    
    //==================================================================================================================
    void SplitPanel::drawBackground(juce::Graphics &parG)
    {
        lookAndFeel->drawSplitPanelBackground(parG, *this);
    }
    
    void SplitPanel::drawBorder(juce::Graphics &parG)
    {
        lookAndFeel->drawSplitPanelBorder(parG, *this);
    }

}
//======================================================================================================================
// endregion SplitPane
//**********************************************************************************************************************
// region LookAndFeel
//======================================================================================================================
namespace jaut
{
    //==================================================================================================================
    void LookAndFeel::drawSplitPanelBackground(juce::Graphics &parG, const SplitPanel &parPanel)
    {
        drawExtraComponentBackground(parG, parPanel, parPanel.findColour(SplitPanel::ColourBackgroundId, true));
    }
    
    void LookAndFeel::drawSplitPanelBorder(juce::Graphics &parG, const SplitPanel &parPanel)
    {
        drawExtraComponentBorder(parG, parPanel, parPanel.findColour(SplitPanel::ColourBorderId, true));
    }
    
    void LookAndFeel::drawSplitPanelSeparator(juce::Graphics &parG, const juce::Component &parSeparator)
    {
        parG.setColour(parSeparator.findColour(SplitPanel::ColourSeparatorId, true));
        parG.fillRect(parSeparator.getBounds());
    }
    
    //==================================================================================================================
    JAUT_INTERN_IMPL_LAF(SplitPanel)
}
//======================================================================================================================
// endregion LookAndFeel
//**********************************************************************************************************************
