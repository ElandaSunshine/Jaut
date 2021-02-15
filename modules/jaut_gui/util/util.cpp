
namespace jaut
{
    //******************************************************************************************************************
    // region OverlayTarget
    //==================================================================================================================
    OverlayTarget::OverlayTarget(juce::Component &targetComponent, int targetBlocksToUse)
        : target(targetComponent), usedBlocks(targetBlocksToUse), resizeAgent(*this)
    {
        target.addComponentListener(&resizeAgent);
    
        for (int i = 0; i < overlayPaths.size(); ++i)
        {
            if ((targetBlocksToUse & i) == i)
            {
                overlayPaths[i].enabled = true;
            }
        }
    }
    
    //==================================================================================================================
    void OverlayTarget::itemDragEnter(const SourceDetails &details)
    {
        if (!isInterestedInDragSource(details))
        {
            return;
        }
        
        paintDropBox = true;
        lastActive   = -1;
        target.repaint();
        onItemEnter(details);
    }
    
    void OverlayTarget::itemDragExit(const SourceDetails &details)
    {
        paintDropBox = false;
        target.repaint();
        onItemLeave(details);
    }
    
    void OverlayTarget::itemDropped(const SourceDetails &dragSourceDetails)
    {
        paintDropBox = false;
        target.repaint();
        onItemDropped(dragSourceDetails);
    }
    
    void OverlayTarget::itemDragMove(const SourceDetails &dragSourceDetails)
    {
        if (!paintDropBox)
        {
            return;
        }
    
        const juce::Point<float> pos = dragSourceDetails.localPosition.toFloat();
    
        if (lastActive > 0 && lastActive < numBlocks)
        {
            const PathElement &element = overlayPaths[lastActive];
        
            if (element.enabled && element.path.contains(pos))
            {
                return;
            }
        }
    
        for (int i = 0; i < overlayPaths.size(); ++i)
        {
            if (i == lastActive)
            {
                continue;
            }
        
            const PathElement &element = overlayPaths[i];
        
            if (element.enabled && element.path.contains(pos))
            {
                lastActive = i;
                target.repaint();
                return;
            }
        }
    }
    
    //==================================================================================================================
    void OverlayTarget::drawDropBox(juce::Graphics &g, juce::Colour colourNormal, juce::Colour colourActive)
    {
        if (!paintDropBox)
        {
            return;
        }
    
        drawDropBoxPaths(g, overlayPaths, colourNormal, colourActive);
    }
    
    //==================================================================================================================
    void OverlayTarget::setOverlayBounds(juce::Rectangle<int> bounds)
    {
        auto temp = getPathsForBlocks(bounds, usedBlocks);
        std::swap(overlayPaths, temp);
        target.repaint();
    }
    
    //==================================================================================================================
    std::array<OverlayTarget::PathElement, OverlayTarget::numBlocks>
    OverlayTarget::getPathsForBlocks(juce::Rectangle<int> bounds, int targetBlocks)
    {
        std::array<PathElement, numBlocks> paths;
        std::bitset<numBlocks> used_blocks { static_cast<unsigned long long>(targetBlocks) };
    
        // Invalid target block type, see TargetBlock
        jassert((fit<int, FitMode::FitRange>(targetBlocks, 0, allBlocks)));
    
        if (used_blocks.test(TargetBlock::Left) || used_blocks.test(TargetBlock::Right))
        {
            const juce::Rectangle<float> rect {
                regionMargin, regionMargin + regionDist, regionSize,
                bounds.getHeight() - (regionMargin + regionDist) * 2.0f
            };
        
            juce::Path path;
            path.startNewSubPath(rect.getX(),              rect.getY());
            path.lineTo         (rect.getX(),              rect.getBottom());
            path.lineTo         (rect.getRight(),          rect.getBottom() - regionSize);
            path.lineTo         (rect.getX() + regionSize, rect.getY()      + regionSize);
            path.closeSubPath();
        
            if (used_blocks.test(TargetBlock::Left))
            {
                paths[TargetBlock::Left].path.addPath(path);
            }
        
            if (used_blocks.test(TargetBlock::Right))
            {
                path.applyTransform(juce::AffineTransform::rotation(M_PI, 0, bounds.getCentreY())
                                        .translated(bounds.getWidth(), 0.0f));
                paths[TargetBlock::Right].path.addPath(path);
            }
        }
    
        if (used_blocks.test(TargetBlock::Top) || used_blocks.test(TargetBlock::Bottom))
        {
            const juce::Rectangle<float> rect {
                regionMargin + regionDist, regionMargin,
                bounds.getWidth() - (regionMargin + regionDist) * 2.0f, regionSize
            };
        
            juce::Path path;
            path.startNewSubPath(rect.getX(),                  rect.getY());
            path.lineTo         (rect.getX()     + regionSize, rect.getBottom());
            path.lineTo         (rect.getRight() - regionSize, rect.getBottom());
            path.lineTo         (rect.getRight(),              rect.getY());
            path.closeSubPath();
        
            if (used_blocks.test(TargetBlock::Top))
            {
                paths[TargetBlock::Top].path.addPath(path);
            }
        
            if (used_blocks.test(TargetBlock::Bottom))
            {
                path.applyTransform(juce::AffineTransform::rotation(M_PI, 0, bounds.getCentreY())
                                        .translated(0.0f, bounds.getHeight()));
                paths[TargetBlock::Bottom].path.addPath(path);
            }
        }
    
        if (used_blocks.test(TargetBlock::Fill))
        {
            paths[TargetBlock::Fill].path.addRectangle(bounds.reduced(regionMargin + regionSize + regionDist));
        }
    
        return paths;
    }
    
    void OverlayTarget::drawDropBoxPaths(juce::Graphics &g, const std::array<PathElement, numBlocks> &elements,
                                         juce::Colour colourNormal, juce::Colour colourActive)
    {
        for (const auto &element : overlayPaths)
        {
            if (element.enabled)
            {
                g.setColour(element.active ? colourActive : colourNormal);
                g.fillPath(element.path);
            }
        }
    }
    //==================================================================================================================
    // endregion OverlayTarget
    //******************************************************************************************************************
}
