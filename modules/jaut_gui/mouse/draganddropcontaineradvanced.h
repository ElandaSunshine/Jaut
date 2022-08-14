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
    @file   draganddropcontaineradvanced.h
    @date   14, August 2020
    
    ===============================================================
 */

#pragma once

namespace jaut
{
class DragAndDropContainerAdvanced
{
public:
    //==================================================================================================================
    enum class DropTargetType
    {
        Target,
        Desktop,
        None
    };
    
    //==================================================================================================================
    /** Utility to find the DragAndDropContainer for a given Component.

        This will search up this component's parent hierarchy looking for the first
        parent component which is a DragAndDropContainer.
    
        It's useful when a component wants to call startDragging but doesn't know
        the DragAndDropContainer it should to use.
    
        Obviously this may return nullptr if it doesn't find a suitable component.
    */
    static DragAndDropContainerAdvanced* findParentDragContainerFor(const juce::Component *childComponent);
    
    /** This performs an asynchronous drag-and-drop of a set of files to some external
        application.
    
        You can call this function in response to a mouseDrag callback, and it will
        use a native operating system drag-and-drop operation to move or copy some
        files to another application.
    
        @param files            a list of filenames to drag
        @param canMoveFiles     if true, the app that receives the files is allowed to move the files to a new location
                                (if this is appropriate). If false, the receiver is expected to make a copy of them.
        @param sourceComponent  normally, JUCE will assume that the component under the mouse is the source component
                                of the drag, but you can use this parameter to override this.
        @param callback         an optional completion callback that will be called when the operation has ended.
    
        @returns                true if the drag operation was successfully started, or false if it failed for some reason
    
        @see performExternalDragDropOfText
    */
    static bool performExternalDragDropOfFiles(const juce::StringArray &files, bool canMoveFiles,
                                               juce::Component *sourceComponent = nullptr,
                                               std::function<void()> callback = nullptr)
    {
        return juce::DragAndDropContainer::performExternalDragDropOfFiles(files, canMoveFiles, sourceComponent,
                                                                          callback);
    }
    
    /** This performs an asynchronous drag-and-drop of a block of text to some external
        application.

        You can call this function in response to a mouseDrag callback, and it will
        use a native operating system drag-and-drop operation to move or copy some
        text to another application.

        @param text             the text to copy
        @param sourceComponent  Normally, JUCE will assume that the component under the mouse is the source component
                                of the drag, but you can use this parameter to override this.
        @param callback         an optional completion callback that will be called when the operation has ended.

        @returns                true if the drag operation was successfully started, or false if it failed for some reason

        @see performExternalDragDropOfFiles
    */
    static bool performExternalDragDropOfText(const juce::String &text, juce::Component *sourceComponent = nullptr,
                                              std::function<void()> callback = nullptr)
    {
        return juce::DragAndDropContainer::performExternalDragDropOfText(text, sourceComponent, callback);
    }
    
    //==================================================================================================================
    /** Creates a DragAndDropContainer.

        The object that derives from this class must also be a Component.
    */
    DragAndDropContainerAdvanced();
    
    /** Destructor. */
    virtual ~DragAndDropContainerAdvanced();
    
    //==================================================================================================================
    /** Begins a drag-and-drop operation.

        This starts a drag-and-drop operation - call it when the user drags the
        mouse in your drag-source component, and this object will track mouse
        movements until the user lets go of the mouse button, and will send
        appropriate messages to DragAndDropTarget objects that the mouse moves
        over.

        findParentDragContainerFor() is a handy method to call to find the
        drag container to use for a component.

        @param sourceDescription                 a string or value to use as the description of the thing being dragged -
                                                 this will be passed to the objects that might be dropped-onto so they can
                                                 decide whether they want to handle it
        @param sourceComponent                   the component that is being dragged
        @param dragImage                         the image to drag around underneath the mouse. If this is a null image,
                                                 a snapshot of the sourceComponent will be used instead.
        @param allowDraggingToOtherJuceWindows   if true, the dragged component will appear as a desktop
                                                 window, and can be dragged to DragAndDropTargets that are the
                                                 children of components other than this one.
        @param imageOffsetFromMouse              if an image has been passed-in, this specifies the offset
                                                 at which the image should be drawn from the mouse. If it isn't
                                                 specified, then the image will be centred around the mouse. If
                                                 an image hasn't been passed-in, this will be ignored.
        @param inputSourceCausingDrag            the mouse input source which started the drag. When calling
                                                 from within a mouseDown or mouseDrag event, you can pass
                                                 MouseEvent::source to this method. If this param is nullptr then JUCE
                                                 will use the mouse input source which is currently dragging. If there
                                                 are several dragging mouse input sources (which can often occur on mobile)
                                                 then JUCE will use the mouseInputSource which is closest to the sourceComponent.
    */
    void startDragging(const juce::var &sourceDescription, juce::Component *sourceComponent,
                       juce::Image dragImage = juce::Image(), bool allowDraggingToOtherJuceWindows = false,
                       const juce::Point<int> *imageOffsetFromMouse = nullptr,
                       const juce::MouseInputSource *inputSourceCausingDrag = nullptr);
    
    //==================================================================================================================
    /** Returns the number of things currently being dragged. */
    int getNumCurrentDrags() const;
    
    /** Returns the description of the thing that's currently being dragged.

        If nothing's being dragged, this will return a null var, otherwise it'll return
        the var that was passed into startDragging().

        If you are using drag and drop in a multi-touch environment then you should use the
        getDragDescriptionForIndex() method instead which takes a touch index parameter.

        @see startDragging, getDragDescriptionForIndex
    */
    juce::var getCurrentDragDescription() const;
    
    /** Same as the getCurrentDragDescription() method but takes a touch index parameter.

        @see getCurrentDragDescription
    */
    juce::var getDragDescriptionForIndex(int index) const;
    
    /** If a drag is in progress, this allows the image being shown to be dynamically updated.

        If you are using drag and drop in a multi-touch environment then you should use the
        setDragImageForIndex() method instead which takes a touch index parameter.

        @see setDragImageForIndex
    */
    void setCurrentDragImage(const juce::Image &newImage);
    
    /** Same as the setCurrentDragImage() method but takes a touch index parameter.

        @see setCurrentDragImage
     */
    void setDragImageForIndex(int index, const juce::Image &newImage);
    
    //==================================================================================================================
    /** Returns true if something is currently being dragged. */
    bool isDragAndDropActive() const;
    
protected:
    /** Override this if you want to be able to perform an external drag of a set of files
        when the user drags outside of this container component.

        This method will be called when a drag operation moves outside the JUCE window,
        and if you want it to then perform a file drag-and-drop, add the filenames you want
        to the array passed in, and return true.

        @param sourceDetails    information about the source of the drag operation
        @param files            on return, the filenames you want to drag
        @param canMoveFiles     on return, true if it's ok for the receiver to move the files; false if
                                it must make a copy of them (see the performExternalDragDropOfFiles() method)
        @see performExternalDragDropOfFiles, shouldDropTextWhenDraggedExternally
    */
    virtual bool shouldDropFilesWhenDraggedExternally(const juce::DragAndDropTarget::SourceDetails &sourceDetails,
                                                      juce::StringArray &files, bool &canMoveFiles);
    
    /** Override this if you want to be able to perform an external drag of text
        when the user drags outside of this container component.

        This method will be called when a drag operation moves outside the JUCE window,
        and if you want it to then perform a text drag-and-drop, copy the text you want to
        be dragged into the argument provided and return true.

        @param sourceDetails    information about the source of the drag operation
        @param text             on return, the text you want to drag
        @see shouldDropFilesWhenDraggedExternally
    */
    virtual bool shouldDropTextWhenDraggedExternally(const juce::DragAndDropTarget::SourceDetails &sourceDetails,
                                                     juce::String &text);
    
    //==================================================================================================================
    /** Subclasses can override this to be told when a drag starts. */
    virtual void dragOperationStarted(const juce::DragAndDropTarget::SourceDetails &sourceDetails) {}
    
    /** Subclasses can override this to be told when a drag finishes. */
    virtual void dragOperationEnded(const juce::DragAndDropTarget::SourceDetails &sourceDetails) {}
    
    //==================================================================================================================
    virtual void dragDropped(const juce::DragAndDropTarget::SourceDetails &sourceDetails, DropTargetType dropType) {}
    
private:
    //==================================================================================================================
    class DragImageComponent;
    
    //==================================================================================================================
    juce::OwnedArray<DragImageComponent> dragImageComponents;
    
    //==================================================================================================================
    const juce::MouseInputSource* getMouseInputSourceForDrag(juce::Component *sourceComponent,
                                                             const juce::MouseInputSource *inputSourceCausingDrag);
    
    //==================================================================================================================
    bool isAlreadyDragging(const juce::Component *sourceComponent) const noexcept;

#if JUCE_CATCH_DEPRECATED_CODE_MISUSE
    // This is just here to cause a compile error in old code that hasn't been changed to use the new
    // version of this method.
    virtual int dragOperationStarted()              { return 0; }
    virtual int dragOperationEnded()                { return 0; }
#endif
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DragAndDropContainerAdvanced)
};
}
