juce_add_modules(
    INSTALL_PATH    "include/jaut/${JAUT_VERSION}"
    ALIAS_NAMESPACE jaut
    
    jaut_audio
    jaut_core
    jaut_gui
    jaut_message
    jaut_logger
    jaut_provider
    jaut_dummy)
