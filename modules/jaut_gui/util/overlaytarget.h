
#pragma once

namespace jaut
{
    class JAUT_API OverlayTarget : public juce::DragAndDropTarget
    {
    private:
        static constexpr float regionMargin = 2.0f;
        static constexpr float regionDist   = 2.0f;
        static constexpr float regionSize   = 30.0f;
        
    public:
        static constexpr int numBlocks = 5;
        static constexpr int allBlocks = 31;
        
        //==============================================================================================================
        struct JAUT_API TargetBlock { enum { Left, Top, Right, Bottom, Fill }; };
    
        //==============================================================================================================
        struct JAUT_API PathElement
        {
            juce::Path path;
            bool enabled { false };
            bool active  { false };
        };
        
        //==============================================================================================================
        /**
         *  Creates a new OverlayTarget instance.
         *
         *  @param targetComponent   The component inheriting from this class
         *  @param targetBlocksToUse The target blocks to use, if you want to use all you can just set this to
         *                           OverlayTarget::allBlocks
         */
        explicit OverlayTarget(juce::Component &targetComponent, int targetBlocksToUse);
        
        //==============================================================================================================
        void itemDragEnter(const SourceDetails &sourceDetails) override;
        void itemDragExit (const SourceDetails &sourceDetails) override;
        void itemDropped  (const SourceDetails &sourceDetails) override;
        void itemDragMove (const SourceDetails &sourceDetails) override;
        
        //==============================================================================================================
        /**
         *  Draws the drop overlay of the target component.
         *  This calls internally drawDropBoxPaths, if you want your custom drawing routine you can override that.
         *
         *  Make sure to call this in your paintOverChildren().
         *
         *  @param g            The graphics context
         *  @param colourNormal The normal colour of a region when not hovered over it
         *  @param colourActive The colour of a region when hovered over it
         */
        void drawDropBox(juce::Graphics &g, juce::Colour colourNormal = juce::Colours::white.withAlpha(0.3f),
                         juce::Colour colourActive = juce::Colours::white.withAlpha(0.7f));
    
        //==============================================================================================================
        /**
         *  Sets the bounds of the component.
         *  Usually this doesn't need to be called explicity as TargetOverlay got an internal ComponentListener,
         *  but just for that one edge-case it is still here.
         *
         *  @param bounds The bounds of the component
         */
        void setOverlayBounds(juce::Rectangle<int> bounds);
        
    protected:
        /**
         *  Creates the paths for the given target blocks.
         *  If you got your own style, you can override this function and provide your own paths to be used.
         *
         *  @param bounds       The bounds of the component
         *  @param targetBlocks The target blocks to be created
         *  @return An array with the created paths
         */
        virtual std::array<PathElement, numBlocks> getPathsForBlocks(juce::Rectangle<int> bounds, int targetBlocks);
        
        /**
         *  Draws the actual overlay box of the drop overlay.
         *  You can override this to make your own drawing routine.
         *
         *  Don't call this directly to paint the overlay box, instead, use drawDropBox.
         *
         *  @param g            The graphics context
         *  @param elements     An array of paths that will be drawn
         *  @param colourNormal The normal colour of a region when not hovered over it
         *  @param colourActive The colour of a region when hovered over it
         */
        virtual void drawDropBoxPaths(juce::Graphics &g, const std::array<PathElement, numBlocks> &elements,
                                      juce::Colour colourNormal, juce::Colour colourActive);
        
        /**
         *  Override this to handle anything that happens when the mouse enters the component with
         *  a drag and drop event.
         *
         *  @param details The source details
         */
        virtual void onItemEnter(const SourceDetails &details) {}
    
        /**
         *  Override this to handle anything that happens when the mouse leaves the component with
         *  a drag and drop event.
         *
         *  @param details The source details
         */
        virtual void onItemLeave(const SourceDetails &details) {}
        
        /**
         *  Override this to handle anything that happens when a drag and drop event was dropped onto the component.
         *  @param details The source details
         */
        virtual void onItemDropped(const SourceDetails &details) = 0;
        
    private:
        class ComponentAgent : public juce::ComponentListener
        {
        public:
            OverlayTarget &overlay;
            
            //==========================================================================================================
            explicit ComponentAgent(OverlayTarget &parOverlay) noexcept
                : overlay(parOverlay)
            {}
            
            //==========================================================================================================
            void componentMovedOrResized(juce::Component &component, bool wasMoved, bool wasResized) override
            {
                overlay.setOverlayBounds(component.getLocalBounds());
            }
        };
        
        //==============================================================================================================
        std::array<PathElement, numBlocks> overlayPaths;
        juce::Component &target;
    
        ComponentAgent resizeAgent;
        
        int  lastActive { -1 };
        int  usedBlocks;
        bool paintDropBox { false };
    
        //==============================================================================================================
        JUCE_DECLARE_NON_COPYABLE(OverlayTarget)
    };
}
