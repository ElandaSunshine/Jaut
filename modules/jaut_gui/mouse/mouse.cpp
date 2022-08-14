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
    @file   mouse.cpp
    @date   08, August 2020

    ===============================================================
 */

namespace jaut
{
//**********************************************************************************************************************
// region DragAndDropContainerAdvanced
//======================================================================================================================
//======================================================================================================================
//**********************************************************************************************************************
// region DragImageComponent
//======================================================================================================================
class DragAndDropContainerAdvanced::DragImageComponent : public juce::Component, private juce::Timer
{
public:
    juce::DragAndDropTarget::SourceDetails sourceDetails;
    
    //==================================================================================================================
    DragImageComponent(const juce::Image &im, const juce::var &desc, Component *const sourceComponent,
                       const juce::MouseInputSource *draggingSource, DragAndDropContainerAdvanced &ddc,
                       juce::Point<int> offset)
        : sourceDetails(desc, sourceComponent, {}), image(im), owner(ddc),
          mouseDragSource(draggingSource->getComponentUnderMouse()), imageOffset(offset),
          originalInputSourceIndex(draggingSource->getIndex()),
          originalInputSourceType(draggingSource->getType())
    {
        updateSize();
        
        if (mouseDragSource == nullptr)
        {
            mouseDragSource = sourceComponent;
        }
        
        mouseDragSource->addMouseListener(this, false);
        
        startTimer(200);
        
        setInterceptsMouseClicks(false, false);
        setAlwaysOnTop(true);
    }
    
    ~DragImageComponent() override
    {
        owner.dragImageComponents.remove(owner.dragImageComponents.indexOf(this), false);
        
        if (mouseDragSource != nullptr)
        {
            mouseDragSource->removeMouseListener(this);
    
            if (auto *const current = getCurrentlyOver())
            {
                if (current->isInterestedInDragSource(sourceDetails))
                {
                    current->itemDragExit(sourceDetails);
                }
            }
        }
        
        owner.dragOperationEnded(sourceDetails);
    }
    
    //==================================================================================================================
    void paint(juce::Graphics &g) override
    {
        if (isOpaque())
        {
            g.fillAll(juce::Colours::white);
        }
        
        g.setOpacity (1.0f);
        g.drawImageAt(image, 0, 0);
    }
    
    //==================================================================================================================
    void mouseUp(const juce::MouseEvent &e) override
    {
        if (e.originalComponent != this && isOriginalInputSource(e.source))
        {
            if (mouseDragSource != nullptr)
            {
                mouseDragSource->removeMouseListener(this);
            }
            
            const juce::Point<int> screen_pos = e.getScreenPosition();
            const bool was_visible            = isVisible();
            
            juce::DragAndDropTarget *final_target;
            juce::Component         *target_comp;
    
            setVisible (false);
            final_target = findTarget(screen_pos, sourceDetails.localPosition, target_comp);
            
            if (was_visible)
            {
                dismissWithAnimation(final_target == nullptr);
            }
            
            if (juce::Component *const parent = getParentComponent())
            {
                parent->removeChildComponent(this);
            }
            
            if (final_target != nullptr)
            {
                currentlyOverComp = nullptr;
                final_target->itemDropped(sourceDetails);
                owner.dragDropped(sourceDetails, DropTargetType::Target);
            }
            else
            {
                currentlyOverComp = nullptr;
                
                if (findDesktopComponentBelow(screen_pos))
                {
                    owner.dragDropped(sourceDetails, DropTargetType::None);
                    return;
                }
                
                owner.dragDropped(sourceDetails, DropTargetType::Desktop);
            }
        }
    }
    
    void mouseDrag(const juce::MouseEvent &e) override
    {
        if (e.originalComponent != this && isOriginalInputSource(e.source))
        {
            updateLocation(true, e.getScreenPosition());
        }
    }
    
    //==================================================================================================================
    bool keyPressed(const juce::KeyPress &key) override
    {
        if (key == juce::KeyPress::escapeKey)
        {
            dismissWithAnimation(true);
            deleteSelf();
            return true;
        }
        
        return false;
    }
    
    //==================================================================================================================
    void updateLocation(const bool canDoExternalDrag, juce::Point<int> screenPos)
    {
        juce::DragAndDropTarget::SourceDetails details = sourceDetails;
        
        setNewScreenPos(screenPos);
        
        juce::Component *new_target_comp;
        juce::DragAndDropTarget *const new_target = findTarget(screenPos, details.localPosition, new_target_comp);
        
        setVisible(!new_target || new_target->shouldDrawDragImageWhenOver());
        
        if (new_target_comp != currentlyOverComp)
        {
            if (juce::DragAndDropTarget *const last_target = getCurrentlyOver())
            {
                if (details.sourceComponent != nullptr && last_target->isInterestedInDragSource(details))
                {
                    last_target->itemDragExit(details);
                }
            }
    
            currentlyOverComp = new_target_comp;
    
            if (new_target && new_target->isInterestedInDragSource(details))
            {
                new_target->itemDragEnter(details);
            }
        }
        
        sendDragMove(details);
        
        if (canDoExternalDrag)
        {
            const juce::Time now = juce::Time::getCurrentTime();
    
            if (getCurrentlyOver() != nullptr)
            {
                lastTimeOverTarget = now;
            }
            else if (now > lastTimeOverTarget + juce::RelativeTime::milliseconds(700))
            {
                checkForExternalDrag(details, screenPos);
            }
        }
        
        forceMouseCursorUpdate();
    }
    
    void updateImage(const juce::Image& newImage)
    {
        image = newImage;
        updateSize();
        repaint();
    }
    
    //==================================================================================================================
    bool canModalEventBeSentToComponent (const juce::Component *targetComponent) override
    {
        return targetComponent == mouseDragSource;
    }
    
    //==================================================================================================================
    void timerCallback() override
    {
        forceMouseCursorUpdate();
        
        if (sourceDetails.sourceComponent == nullptr)
        {
            deleteSelf();
        }
        else
        {
            for (const juce::MouseInputSource &s : juce::Desktop::getInstance().getMouseSources())
            {
                if (isOriginalInputSource(s) && ! s.isDragging())
                {
                    if (mouseDragSource != nullptr)
                    {
                        mouseDragSource->removeMouseListener(this);
                    }
                    
                    deleteSelf();
                    break;
                }
            }
        }
    }
    
    //==================================================================================================================
    void inputAttemptWhenModal() override {}
    
private:
    static void forceMouseCursorUpdate()
    {
        juce::Desktop::getInstance().getMainMouseSource().forceMouseCursorUpdate();
    }
    
    static juce::Component* findDesktopComponentBelow(juce::Point<int> screenPos)
    {
        const juce::Desktop &desktop = juce::Desktop::getInstance();
        
        for (int i = desktop.getNumComponents(); --i >= 0;)
        {
            juce::Component *const desktop_component = desktop.getComponent(i);
            juce::Point<int> desk_point              = desktop_component->getLocalPoint(nullptr, screenPos);
            
            if (juce::Component *const target_comp = desktop_component->getComponentAt(desk_point))
            {
                const juce::Point<int> comp_point = target_comp->getLocalPoint(desktop_component, desk_point);
                
                if (target_comp->hitTest(comp_point.getX(), comp_point.getY()))
                {
                    return target_comp;
                }
            }
        }
        
        return nullptr;
    }
    
    //==================================================================================================================
    juce::Image image;
    DragAndDropContainerAdvanced &owner;
    juce::WeakReference<Component> mouseDragSource, currentlyOverComp;
    const juce::Point<int> imageOffset;
    bool hasCheckedForExternalDrag { false };
    juce::Time lastTimeOverTarget;
    int originalInputSourceIndex;
    juce::MouseInputSource::InputSourceType originalInputSourceType;
    
    //==================================================================================================================
    void updateSize()
    {
        setSize(image.getWidth(), image.getHeight());
    }
    
    //==================================================================================================================
    juce::DragAndDropTarget* getCurrentlyOver() const noexcept
    {
        return dynamic_cast<juce::DragAndDropTarget*>(currentlyOverComp.get());
    }
    
    juce::DragAndDropTarget* findTarget(juce::Point<int> screenPos, juce::Point<int>& relativePos,
                                        juce::Component*& resultComponent) const
    {
        juce::Component *hit_comp = getParentComponent();
        
        if (hit_comp == nullptr)
        {
            hit_comp = findDesktopComponentBelow(screenPos);
        }
        else
        {
            hit_comp = hit_comp->getComponentAt(hit_comp->getLocalPoint(nullptr, screenPos));
        }
        
        while (hit_comp)
        {
            if (auto *const ddt = dynamic_cast<juce::DragAndDropTarget*>(hit_comp))
            {
                if (ddt->isInterestedInDragSource(sourceDetails))
                {
                    relativePos     = hit_comp->getLocalPoint(nullptr, screenPos);
                    resultComponent = hit_comp;
                    
                    return ddt;
                }
            }
            
            hit_comp = hit_comp->getParentComponent();
        }
        
        resultComponent = nullptr;
        return nullptr;
    }
    
    void setNewScreenPos(juce::Point<int> screenPos)
    {
        juce::Point<int> new_pos = screenPos - imageOffset;
        
        if (const juce::Component *const parent = getParentComponent())
        {
            new_pos = parent->getLocalPoint(nullptr, new_pos);
        }
        
        setTopLeftPosition(new_pos);
    }
    
    void sendDragMove(const juce::DragAndDropTarget::SourceDetails &details) const
    {
        if (juce::DragAndDropTarget *const target = getCurrentlyOver())
        {
            if (target->isInterestedInDragSource(details))
            {
                target->itemDragMove(details);
            }
        }
    }
    
    void checkForExternalDrag(const juce::DragAndDropTarget::SourceDetails &details, juce::Point<int> screenPos)
    {
        if (!hasCheckedForExternalDrag)
        {
            if (!juce::Desktop::getInstance().findComponentAt(screenPos))
            {
                hasCheckedForExternalDrag = true;
                
                if (juce::ComponentPeer::getCurrentModifiersRealtime().isAnyMouseButtonDown())
                {
                    juce::StringArray files;
                    bool can_move_files = false;
                    
                    if (owner.shouldDropFilesWhenDraggedExternally(details, files, can_move_files) && !files.isEmpty())
                    {
                        juce::MessageManager::callAsync([=]
                        {
                            DragAndDropContainerAdvanced::performExternalDragDropOfFiles(files, can_move_files);
                        });
                        
                        deleteSelf();
                        return;
                    }
                    
                    juce::String text;
                    
                    if (owner.shouldDropTextWhenDraggedExternally (details, text) && text.isNotEmpty())
                    {
                        juce::MessageManager::callAsync([=]
                        {
                            DragAndDropContainerAdvanced::performExternalDragDropOfText(text);
                        });
                        
                        deleteSelf();
                        return;
                    }
                }
            }
        }
    }
    
    void deleteSelf()
    {
        delete this;
    }
    
    void dismissWithAnimation(bool shouldSnapBack)
    {
        juce::ComponentAnimator &animator = juce::Desktop::getInstance().getAnimator();
        setVisible(true);
    
        if (shouldSnapBack && sourceDetails.sourceComponent)
        {
            const juce::Point<int> target = sourceDetails.sourceComponent
                                                        ->localPointToGlobal(sourceDetails.sourceComponent
                                                                                         ->getLocalBounds()
                                                                                          .getCentre());
            const juce::Point<int> our_centre = localPointToGlobal(getLocalBounds().getCentre());
            animator.animateComponent(this, getBounds() + (target - our_centre), 0.0f, 120, true, 1.0, 1.0);
        }
        else
        {
            animator.fadeOut(this, 120);
        }
    }
    
    bool isOriginalInputSource(const juce::MouseInputSource &sourceToCheck)
    {
        return (sourceToCheck.getType() == originalInputSourceType
                && sourceToCheck.getIndex() == originalInputSourceIndex);
    }
    
    JUCE_DECLARE_NON_COPYABLE(DragImageComponent)
};
//======================================================================================================================
// endregion DragImageComponent
//**********************************************************************************************************************
// region DragAndDropContainerAdvanced
//======================================================================================================================
DragAndDropContainerAdvanced* DragAndDropContainerAdvanced::findParentDragContainerFor(const juce::Component *comp)
{
    return comp != nullptr ? comp->findParentComponentOfClass<DragAndDropContainerAdvanced>() : nullptr;
}

//======================================================================================================================
DragAndDropContainerAdvanced::DragAndDropContainerAdvanced()
{}

DragAndDropContainerAdvanced::~DragAndDropContainerAdvanced()
{}

//======================================================================================================================
void DragAndDropContainerAdvanced::startDragging(const juce::var &sourceDescription, juce::Component *sourceComponent,
                                                 juce::Image dragImage, bool allowDraggingToExternalWindows,
                                                 const juce::Point<int> *imageOffsetFromMouse,
                                                 const juce::MouseInputSource *inputSourceCausingDrag)
{
    if (isAlreadyDragging (sourceComponent))
    {
        return;
    }
    
    const juce::MouseInputSource *const draggingSource
                         = getMouseInputSourceForDrag(sourceComponent, inputSourceCausingDrag);
    
    if (draggingSource == nullptr || !draggingSource->isDragging())
    {
        jassertfalse;
        return;
    }
    
    const juce::Point<int> last_mouse_down = draggingSource->getLastMouseDownPosition().roundToInt();
    juce::Point<int> image_offset;
    
    if (dragImage.isNull())
    {
        dragImage = sourceComponent->createComponentSnapshot (sourceComponent->getLocalBounds())
                                   .convertedToFormat (juce::Image::ARGB);
        
        dragImage.multiplyAllAlphas (0.6f);
        
        constexpr int lo = 150;
        constexpr int hi = 400;
        
        const juce::Point<int> rel_pos = sourceComponent->getLocalPoint(nullptr, last_mouse_down);
        const juce::Point<int> clipped = dragImage.getBounds().getConstrainedPoint(rel_pos);
        juce::Random random;
        
        for (int y = dragImage.getHeight(); --y >= 0;)
        {
            const int dy = (y - clipped.getY()) * (y - clipped.getY());
            
            for (int x = dragImage.getWidth(); --x >= 0;)
            {
                const int dx       = x - clipped.getX();
                const int distance = juce::roundToInt(std::sqrt(dx * dx + dy));
                
                if (distance > lo)
                {
                    const float alpha = (distance > hi) ? 0.0f : static_cast<float>(hi - distance)
                                                                 / static_cast<float>(hi - lo)
                                                                 + random.nextFloat() * 0.008f;
                    dragImage.multiplyAlphaAt(x, y, alpha);
                }
            }
        }
    
        image_offset = clipped;
    }
    else
    {
        if (imageOffsetFromMouse == nullptr)
        {
            image_offset = dragImage.getBounds().getCentre();
        }
        else
        {
            image_offset = dragImage.getBounds().getConstrainedPoint(-*imageOffsetFromMouse);
        }
    }
    
    DragImageComponent *const dragImageComponent = dragImageComponents.add(new DragImageComponent(dragImage,
                                                                                                  sourceDescription,
                                                                                                  sourceComponent,
                                                                                                  draggingSource,
                                                                                                  *this,
                                                                                                  image_offset));
    
    if (allowDraggingToExternalWindows)
    {
        if (!juce::Desktop::canUseSemiTransparentWindows())
        {
            dragImageComponent->setOpaque(true);
        }
        
        dragImageComponent->addToDesktop(juce::ComponentPeer::windowIgnoresMouseClicks
                                         | juce::ComponentPeer::windowIsTemporary
                                         | juce::ComponentPeer::windowIgnoresKeyPresses);
    }
    else
    {
        if (auto *const this_comp = dynamic_cast<juce::Component*>(this))
        {
            this_comp->addChildComponent(dragImageComponent);
        }
        else
        {
            jassertfalse;   // Your DragAndDropContainerAdvanced needs to be a Component!
            return;
        }
    }
    
    dragImageComponent->sourceDetails.localPosition = sourceComponent->getLocalPoint(nullptr, last_mouse_down);
    dragImageComponent->updateLocation(false, last_mouse_down);

#if JUCE_WINDOWS
    if (juce::ComponentPeer *const peer = dragImageComponent->getPeer())
    {
        peer->performAnyPendingRepaintsNow();
    }
#endif
    
    dragOperationStarted(dragImageComponent->sourceDetails);
}

//======================================================================================================================
int DragAndDropContainerAdvanced::getNumCurrentDrags() const
{
    return dragImageComponents.size();
}

juce::var DragAndDropContainerAdvanced::getCurrentDragDescription() const
{
    jassert(dragImageComponents.size() < 2);
    return dragImageComponents.size() != 0 ? dragImageComponents[0]->sourceDetails.description : juce::var();
}

juce::var DragAndDropContainerAdvanced::getDragDescriptionForIndex(int index) const
{
    if (!jaut::fit(index, 0, dragImageComponents.size()))
    {
        return {};
    }
    
    return dragImageComponents.getUnchecked(index)->sourceDetails.description;
}

void DragAndDropContainerAdvanced::setCurrentDragImage(const juce::Image &newImage)
{
    jassert(dragImageComponents.size() < 2);
    dragImageComponents[0]->updateImage(newImage);
}

void DragAndDropContainerAdvanced::setDragImageForIndex(int index, const juce::Image &newImage)
{
    if (jaut::fit(index, 0, dragImageComponents.size()))
    {
        dragImageComponents.getUnchecked(index)->updateImage(newImage);
    }
}

//======================================================================================================================
bool DragAndDropContainerAdvanced::isDragAndDropActive() const
{
    return dragImageComponents.size() > 0;
}

//======================================================================================================================
bool DragAndDropContainerAdvanced::shouldDropFilesWhenDraggedExternally(const juce::DragAndDropTarget::SourceDetails&,
                                                                        juce::StringArray&, bool&)
{
    return false;
}

bool DragAndDropContainerAdvanced::shouldDropTextWhenDraggedExternally(const juce::DragAndDropTarget::SourceDetails&,
                                                                       juce::String&)
{
    return false;
}

//======================================================================================================================
const juce::MouseInputSource*
      DragAndDropContainerAdvanced::getMouseInputSourceForDrag(juce::Component              *sourceComponent,
                                                               const juce::MouseInputSource *inputSourceCausingDrag)
{
    if (inputSourceCausingDrag == nullptr)
    {
        float min_distance     = std::numeric_limits<float>::max();
        juce::Desktop &desktop = juce::Desktop::getInstance();
        
        const juce::Point<float> centre_point = sourceComponent
                                                    ? sourceComponent->getScreenBounds().getCentre().toFloat()
                                                    : juce::Point<float>();
        const int num_dragging = desktop.getNumDraggingMouseSources();
        
        for (auto i = 0; i < num_dragging; ++i)
        {
            if (const juce::MouseInputSource *const ms = desktop.getDraggingMouseSource(i))
            {
                const float distance = ms->getScreenPosition().getDistanceSquaredFrom(centre_point);
                
                if (distance < min_distance)
                {
                    min_distance           = distance;
                    inputSourceCausingDrag = ms;
                }
            }
        }
    }
    
    jassert(inputSourceCausingDrag != nullptr && inputSourceCausingDrag->isDragging());
    return inputSourceCausingDrag;
}

//======================================================================================================================
bool DragAndDropContainerAdvanced::isAlreadyDragging(const juce::Component *component) const noexcept
{
    for (const DragImageComponent *const dragImageComp : dragImageComponents)
    {
        if (dragImageComp->sourceDetails.sourceComponent == component)
        {
            return true;
        }
    }
    
    return false;
}
//======================================================================================================================
// endregion DragAndDropContainerAdvanced
//**********************************************************************************************************************
//======================================================================================================================
//======================================================================================================================
// endregion DragAndDropContainerAdvanced
//**********************************************************************************************************************
}
