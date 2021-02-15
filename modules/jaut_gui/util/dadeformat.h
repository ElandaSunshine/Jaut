
#pragma

namespace jaut
{
    struct DadeFormat
    {
        /** Whether the component contained in the drag and drop event is owned by the drag and drop event. */
        bool componentOwned;
        
        /**
         *  Whether the component contained in the drag and drop event is just a reference component or a temporary.
         *  Temporary component don't belong to any other component and are meant to add to the target component.
         */
        bool componentDropped;
        
        /**
         *  The component which has been set as the drag component.
         *  Can either be owned or non owned.
         */
        juce::OptionalScopedPointer<juce::Component> eventComponent;
        
        
    };
}
