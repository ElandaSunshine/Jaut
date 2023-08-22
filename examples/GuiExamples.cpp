/**
    ─────────────────────────────── ⋆⋅☆⋅⋆ ───────────────────────────────
                     ░░░░░██╗░█████╗░██╗░░░██╗████████╗
                     ░░░░░██║██╔══██╗██║░░░██║╚══██╔══╝
                     ░░░░░██║███████║██║░░░██║░░░██║░░░
                     ██╗░░██║██╔══██║██║░░░██║░░░██║░░░
                     ╚█████╔╝██║░░██║╚██████╔╝░░░██║░░░
                     ░╚════╝░╚═╝░░╚═╝░╚═════╝░░░░╚═╝░░░
                       JUCE Augmented Utility  Toolbox
    ─────────────────────────────── ⋆⋅☆⋅⋆ ───────────────────────────────
    
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
    @file   SplitPane.cpp
    @date   09, March 2023

    ===============================================================
 */

#include "GuiExamples.h"

#include <jaut_gui/component/jaut_SplitPanel.h>



//**********************************************************************************************************************
// region Namespace
//======================================================================================================================
namespace
{
    //==================================================================================================================
    class DistancePropertyComponent : public juce::PropertyComponent
    {
    public:
        using DistanceMemberPtr = jaut::MemberDataPointer_t<jaut::Distance<int>, int>;
        
        //==============================================================================================================
        using SetDistanceHandler = jaut::EventHandler<int, DistanceMemberPtr>;
        
        jaut::Event<SetDistanceHandler> eventSetDistance;
        
        //==============================================================================================================
        DistancePropertyComponent(const juce::String                          &parPropertyName,
                                  jaut::Distance<int>                         parInitValue,
                                  std::function<void(int, DistanceMemberPtr)> parSetterFunc)
            : juce::PropertyComponent(parPropertyName, 25)
        {
            eventSetDistance += jaut::makeHandler(std::move(parSetterFunc));
            
            static constexpr std::array params {
                &jaut::Distance<int>::left,
                &jaut::Distance<int>::top,
                &jaut::Distance<int>::right,
                &jaut::Distance<int>::bottom
            };
            
            for (std::size_t i = 0; i < labels.size(); ++i)
            {
                juce::Label &label = labels[i];
                
                label.setEditable(true);
                label.onTextChange = [&e = eventSetDistance, &label, param = params[i]]()
                {
                    const juce::String text = label.getText();
            
                    if (!text.containsOnly("-0123456789")
                        || (!text.matchesWildcard("-*", false) && !text.matchesWildcard("*", false)))
                    {
                        label.setText("0", juce::sendNotification);
                        return;
                    }
                    
                    e.invoke(text.getIntValue(), param);
                };
                
                label.setText(juce::String(parInitValue.*(params[i])), juce::dontSendNotification);
                addAndMakeVisible(label);
            }
        }
        
        //==============================================================================================================
        void resized() override
        {
            const int text_w = std::min(200, getWidth() / 2);
            const int height = getHeight() - 3;
            int       x      = text_w;
            
            for (juce::Label &label : labels)
            {
                label.setBounds(x, 1, height + 5, height);
                x += height + 5;
            }
        }
        
        //==============================================================================================================
        void refresh() override {}
        
    private:
        std::array<juce::Label, 4> labels;
    };
    
    class SizePropertyComponent : public juce::PropertyComponent
    {
    public:
        using SizeMemberPtr = jaut::MemberDataPointer_t<jaut::Size<int>, int>;
        
        //==============================================================================================================
        using SetDistanceHandler = jaut::EventHandler<int, SizeMemberPtr>;
        
        jaut::Event<SetDistanceHandler> eventSetSize;
        
        //==============================================================================================================
        SizePropertyComponent(const juce::String                      &parPropertyName,
                              jaut::Size<int>                         parInitValue,
                              std::function<void(int, SizeMemberPtr)> parSetterFunc)
            : juce::PropertyComponent(parPropertyName, 25)
        {
            eventSetSize += jaut::makeHandler(std::move(parSetterFunc));
            
            static constexpr std::array params {
                &jaut::Size<int>::width,
                &jaut::Size<int>::height
            };
            
            for (std::size_t i = 0; i < labels.size(); ++i)
            {
                juce::Label &label = labels[i];
                
                label.setEditable(true);
                label.onTextChange = [&e = eventSetSize, &label, param = params[i]]()
                {
                    const juce::String text = label.getText();
            
                    if (!text.containsOnly("0123456789") || !text.matchesWildcard("*", false))
                    {
                        label.setText("0", juce::sendNotification);
                        return;
                    }
                    
                    e.invoke(text.getIntValue(), param);
                };
                
                label.setText(juce::String(parInitValue.*(params[i])), juce::dontSendNotification);
                addAndMakeVisible(label);
            }
        }
        
        //==============================================================================================================
        void resized() override
        {
            const int text_w = std::min(200, getWidth() / 2);
            const int height = getHeight() - 3;
            int       x      = text_w;
            
            for (juce::Label &label : labels)
            {
                label.setBounds(x, 1, height + 5, height);
                x += height + 5;
            }
        }
        
        //==============================================================================================================
        void refresh() override {}
        
    private:
        std::array<juce::Label, 2> labels;
    };
    
    template<class T>
    class NumberPropertyComponent : public juce::PropertyComponent
    {
    public:
        using SetDistanceHandler = jaut::EventHandler<T>;
        
        jaut::Event<SetDistanceHandler> eventSetValue;
        
        //==============================================================================================================
        NumberPropertyComponent(const juce::String     &parPropertyName,
                                T                      parInitValue,
                                std::function<void(T)> parSetterFunc)
            : juce::PropertyComponent(parPropertyName, 25)
        {
            eventSetValue += jaut::makeHandler(std::move(parSetterFunc));
            
            label.setEditable(true);
            label.onTextChange = [&e = eventSetValue, &label = label]()
            {
                const juce::String text = label.getText();
                
                if (!text.containsOnly("-0123456789.")
                    || (!text.matchesWildcard("-*", false) && !text.matchesWildcard("*", false))
                    && (!text.matchesWildcard("-*.*", false) && !text.matchesWildcard("*.*", false)))
                {
                    label.setText("0", juce::sendNotification);
                    return;
                }
                
                e.invoke(juce::var(text));
            };
            
            label.setText(juce::var(parInitValue).toString(), juce::dontSendNotification);
            addAndMakeVisible(label);
        }
        
        //==============================================================================================================
        void refresh() override {}
        
    private:
        juce::Label label;
    };
    
    class CheckboxPropertyComponent : public juce::PropertyComponent
    {
    public:
        using SetValueHandler = jaut::EventHandler<bool>;
        
        jaut::Event<SetValueHandler> eventSetValue;
        
        //==============================================================================================================
        CheckboxPropertyComponent(const juce::String        &parPropertyName,
                                  bool                      parInitValue,
                                  std::function<void(bool)> parSetterFunc)
            : juce::PropertyComponent(parPropertyName, 25)
        {
            eventSetValue += jaut::makeHandler(std::move(parSetterFunc));
            button.setToggleState(parInitValue, juce::dontSendNotification);
            button.onStateChange = [&e = eventSetValue, &button = button]()
            {
                e.invoke(button.getToggleState());
            };
            
            addAndMakeVisible(button);
        }
        
        //==============================================================================================================
        void refresh() override {}
        
    private:
        juce::ToggleButton button;
    };
    
    class OrientationPropertyComponent : public juce::ChoicePropertyComponent
    {
    public:
        using SetOrientationHandler = jaut::EventHandler<jaut::SplitPanel::Orientation>;
        
        jaut::Event<SetOrientationHandler> eventSetOrientation;
        
        //==============================================================================================================
        OrientationPropertyComponent(const juce::String                                 &parName,
                                     jaut::SplitPanel::Orientation                      parInitValue,
                                     std::function<void(jaut::SplitPanel::Orientation)> parFunc)
            : juce::ChoicePropertyComponent(parName),
              index(static_cast<int>(parInitValue))
        {
            choices = { "Vertical", "Horizontal" };
            eventSetOrientation += jaut::makeHandler(std::move(parFunc));
            juce::ChoicePropertyComponent::refresh();
        }
        
        //==============================================================================================================
        void setIndex(int parNewIndex) override
        {
            index = parNewIndex;
            eventSetOrientation.invoke(static_cast<jaut::SplitPanel::Orientation>(parNewIndex));
        }
        
        JAUT_NODISCARD
        int getIndex() const override
        {
            return index;
        }
        
    private:
        int index;
    };
    
    template<class T>
    class NumberActionPropertyComponent : public juce::PropertyComponent
    {
    public:
        using SetDistanceHandler = jaut::EventHandler<T>;
        
        jaut::Event<SetDistanceHandler> eventSetValue;
        
        //==============================================================================================================
        NumberActionPropertyComponent(const juce::String     &parPropertyName,
                                      T                      parInitValue,
                                      T                      parMin,
                                      T                      parMax,
                                      std::function<void(T)> parSetterFunc)
            : juce::PropertyComponent(parPropertyName, 25),
              min(parMin),
              max(parMax)
        {
            eventSetValue += jaut::makeHandler(std::move(parSetterFunc));
            
            label.setEditable(true);
            label.onTextChange = [this, init = parInitValue]()
            {
                const juce::String text = label.getText();
                
                if (!text.containsOnly("-0123456789.")
                    || (!text.matchesWildcard("-*", false) && !text.matchesWildcard("*", false))
                    && (!text.matchesWildcard("-*.*", false) && !text.matchesWildcard("*.*", false)))
                {
                    label.setText(juce::var(init).toString(), juce::dontSendNotification);
                    return;
                }
                
                const T value = std::clamp<T>(juce::var(text), min, max);
                
                eventSetValue.invoke(value);
                label.setText(juce::var(init), juce::dontSendNotification);
            };
            
            label.setText(juce::var(parInitValue).toString(), juce::dontSendNotification);
            addAndMakeVisible(label);
        }
        
        //==============================================================================================================
        void refresh() override {}
        
    private:
        juce::Label label;
        T min;
        T max;
    };
    
    template<class T>
    class SplitPanelSeparatorPosPropertyComponent : public juce::PropertyComponent,
                                                    private juce::ComponentListener
    {
    public:
        using SetDistanceHandler = jaut::EventHandler<T>;
        
        jaut::Event<SetDistanceHandler> eventSetValue;
        std::function<T()> getterFunc;
        
        //==============================================================================================================
        SplitPanelSeparatorPosPropertyComponent(const juce::String     &parPropertyName,
                                                T                      parInitValue,
                                                T                      parMin,
                                                T                      parMax,
                                                jaut::SplitPanel       &parSplitPanel,
                                                std::function<void(T)> parSetterFunc,
                                                std::function<T()>     parGetterFunc)
            : juce::PropertyComponent(parPropertyName, 25),
              getterFunc(parGetterFunc),
              panel(parSplitPanel),
              min(parMin),
              max(parMax)
        {
            eventSetValue += jaut::makeHandler(std::move(parSetterFunc));
            panel.addComponentListener(this);
            
            parSplitPanel.eventSeparatorMoved += jaut::makeHandler("plist_" + parPropertyName,
                [&label = label, func = parGetterFunc](int,int)
                {
                    label.setText(
                        juce::var(static_cast<T>(std::round(func() * 10000) / 10000)).toString().substring(0, 6),
                        juce::dontSendNotification
                    );
                });
            
            label.setEditable(true);
            label.onTextChange = [this, init = parInitValue]()
            {
                const juce::String text = label.getText();
                
                if (!text.containsOnly("-0123456789.")
                    || (!text.matchesWildcard("-*", false) && !text.matchesWildcard("*", false))
                    && (!text.matchesWildcard("-*.*", false) && !text.matchesWildcard("*.*", false)))
                {
                    label.setText(
                        juce::var(static_cast<T>(std::round(init * 10000) / 10000)).toString().substring(0, 6),
                        juce::dontSendNotification
                    );
                    return;
                }
                
                const T value = std::clamp<T>(juce::var(text), min, max);
                eventSetValue.invoke(value);
            };
            
            label.setText(
                juce::var(static_cast<T>(std::round(parInitValue * 10000) / 10000)).toString().substring(0, 6),
                juce::dontSendNotification
            );
            addAndMakeVisible(label);
        }
        
        ~SplitPanelSeparatorPosPropertyComponent() override
        {
            panel.eventSeparatorMoved -= jaut::makeHandler("plist_" + getName(), [](int,int){});
            panel.removeComponentListener(this);
        }
        
        //==============================================================================================================
        void refresh() override {}
        
    private:
        juce::Label label;
        jaut::SplitPanel &panel;
        T min;
        T max;
        
        //==============================================================================================================
        void componentMovedOrResized(Component&, bool, bool) override
        {
            label.setText(
                juce::var(static_cast<T>(std::round(getterFunc() * 10000) / 10000)).toString().substring(0, 6),
                juce::dontSendNotification
            );
        }
    };
    
    //==================================================================================================================
    using Scheme = jaut::LookAndFeel::ColourScheme::UIColour;
    
    //==================================================================================================================
    template<class T, class ...Args>
    std::unique_ptr<T> makeComponent(Args &&...args)
    {
        auto comp = std::make_unique<T>(std::forward<Args>(args)...);
        comp->setName(jaut::getActualTypeName<T>());
        return comp;
    }
    
    juce::Array<juce::PropertyComponent*> getBasicPropertyComponents(jaut::ExtraComponent &component)
    {
        return {
            new ::DistancePropertyComponent("margin", component.getMargin(),
                                            [&component](auto &&value, auto &&ptr)
                                            {
                                                jaut::Margin<int> margin = component.getMargin();
                                                margin.*ptr = value;
                                                component.setMargin(margin);
                                            }),
            new ::DistancePropertyComponent("borderThickness", component.getBorderThickness(),
                                            [&component](auto &&value, auto &&ptr)
                                            {
                                                jaut::Thickness<int> thickness = component.getBorderThickness();
                                                thickness.*ptr = value;
                                                component.setBorderThickness(thickness);
                                            }),
            new ::DistancePropertyComponent("padding", component.getPadding(),
                                            [&component](auto &&value, auto &&ptr)
                                            {
                                                jaut::Padding<int> padding = component.getPadding();
                                                padding.*ptr = value;
                                                component.setPadding(padding);
                                            }),
        };
    }
}

//======================================================================================================================
std::unique_ptr<juce::Component> getMainGuiComponent()
{
    return std::make_unique<GuiExamples>();
}
//======================================================================================================================
// endregion Namespace
//**********************************************************************************************************************
// region GuiExamples::ExampleModel
//======================================================================================================================
class GuiExamples::ExampleModel : public juce::ListBoxModel
{
public:
    using RowSelectedHandler = jaut::EventHandler<int>;
    
    //==================================================================================================================
    std::vector<std::unique_ptr<jaut::ExtraComponent>> examples;
    jaut::Event<RowSelectedHandler> eventRowSelected;
    jaut::LookAndFeel               &laf;
    
    //==================================================================================================================
    ExampleModel(jaut::LookAndFeel &parLaf, std::vector<std::unique_ptr<jaut::ExtraComponent>> parExamples) noexcept
        : laf     (parLaf),
          examples(std::move(parExamples))
    {}
    
    //==================================================================================================================
    int getNumRows() noexcept override
    {
        return static_cast<int>(examples.size());
    }
    
    void paintListBoxItem(int            parRowNumber,
                          juce::Graphics &parG,
                          int            parWidth,
                          int            parHeight,
                          bool           parRowIsSelected) override
    {
        if (parRowNumber >= getNumRows())
        {
            return;
        }
        
        const juce::Rectangle bounds(0, 0, parWidth, parHeight);
        const jaut::LookAndFeel::ColourScheme &scheme = laf.getCurrentColourScheme();
        
        if (parRowIsSelected)
        {
            parG.setColour(scheme.getUIColour(Scheme::defaultText).withAlpha(0.2f));
            parG.fillRect(bounds);
        }
        
        parG.setColour(scheme.getUIColour(Scheme::defaultText));
        parG.drawFittedText(examples[parRowNumber]->getName(), bounds, juce::Justification::centred, 1);
    }
    
    //==================================================================================================================
    void listBoxItemClicked(int parRow, const juce::MouseEvent&) override
    {
        eventRowSelected.invoke(parRow);
    }
};
//======================================================================================================================
// endregion GuiExamples::ExampleModel
//**********************************************************************************************************************
// region GuiExamples
//======================================================================================================================
GuiExamples::GuiExamples()
    : exampleModel(std::make_unique<ExampleModel>(style, createExamples())),
      exampleList ("Examples List", exampleModel.get())
{
    setSize(1280, 720);
    
    setLookAndFeel(&style);
    
    exampleModel->eventRowSelected += jaut::makeHandler(&GuiExamples::rowSelected, this);
    addAndMakeVisible(exampleList);
    exampleList.updateContent();
    
    addAndMakeVisible(viewBox);
    
    addAndMakeVisible(propertyPanel);
}

GuiExamples::~GuiExamples()
{
    setLookAndFeel(nullptr);
}

//======================================================================================================================
void GuiExamples::paint(juce::Graphics &parG)
{
    const jaut::LookAndFeel::ColourScheme &scheme = style.getCurrentColourScheme();
    parG.setColour(scheme.getUIColour(Scheme::windowBackground));
    parG.fillAll();
    
    parG.setColour(juce::Colours::white.darker(0.2f));
    parG.drawRect(exampleList.getWidth(), 0, 2, getHeight(), 2);
}

void GuiExamples::resized()
{
    juce::Rectangle example_bounds = getLocalBounds();
    juce::Rectangle exlist_bounds  = example_bounds.removeFromLeft (150);
    juce::Rectangle param_bounds   = example_bounds.removeFromRight(300);
    
    exampleList  .setBounds(exlist_bounds);
    viewBox      .setBounds(example_bounds.withTrimmedLeft(2));
    propertyPanel.setBounds(param_bounds);
    
    if (juce::Component *const example_comp = viewBox.getChildComponent(0))
    {
        example_comp->setBounds(viewBox.getLocalBounds());
    }
}

//======================================================================================================================
std::vector<std::unique_ptr<jaut::ExtraComponent>> GuiExamples::createExamples()
{
    std::vector<std::unique_ptr<jaut::ExtraComponent>> out;
    
    {
        auto pane = ::makeComponent<jaut::ContentPane>();
        pane->setComponent(std::make_unique<juce::TextButton>("ContentPaneButton"));
        
        out.emplace_back(std::move(pane));
    }
    
    {
        auto panel = ::makeComponent<jaut::SplitPanel>();
        panel->getPane1().setComponent(std::make_unique<juce::TextButton>("Panel1"));
        panel->getPane2().setComponent(std::make_unique<juce::TextButton>("Panel2"));
        
        out.emplace_back(std::move(panel));
    }
    
    return out;
}

//======================================================================================================================
void GuiExamples::rowSelected(int parRow)
{
    jaut::ExtraComponent &component = *exampleModel->examples[static_cast<std::size_t>(parRow)];
    
    viewBox.removeAllChildren();
    viewBox.addAndMakeVisible(component);
    
    propertyPanel.clear();
    
    juce::Array<juce::PropertyComponent*> style_properties = ::getBasicPropertyComponents(component);
    
    if (auto *const panel = dynamic_cast<jaut::SplitPanel*>(&component))
    {
        style_properties.add(new ::NumberPropertyComponent<int>("separatorThickness", panel->getSeparatorThickness(),
            [panel](int value)
            {
                panel->setSeparatorThickness(value);
            }));
        
        propertyPanel.addSection("Options", {
            new ::SizePropertyComponent("minSizePane1", panel->getMinSizePane1(),
                [panel](int value, SizePropertyComponent::SizeMemberPtr ptr)
                {
                    jaut::Size<int> size = panel->getMinSizePane1();
                    size.*ptr = value;
                    panel->setMinSizePane1(size);
                }),
            new ::SizePropertyComponent("minSizePane2", panel->getMinSizePane2(),
                [panel](int value, SizePropertyComponent::SizeMemberPtr ptr)
                {
                    jaut::Size<int> size = panel->getMinSizePane2();
                    size.*ptr = value;
                    panel->setMinSizePane2(size);
                }),
            new ::OrientationPropertyComponent("orientation", panel->getOrientation(),
                [panel](jaut::SplitPanel::Orientation orientation)
                {
                    panel->setOrientation(orientation);
                }),
            new ::NumberPropertyComponent<double>("resizeDistributionFactor", panel->getResizeDistributionFactor(),
                [panel](double value)
                {
                    panel->setResizeDistributionFactor(value);
                }),
            new ::CheckboxPropertyComponent("collapsePane1", panel->getIsPane1Collapsed(),
                [panel](bool shouldCollapse)
                {
                    panel->setPane1Collapsed(shouldCollapse);
                }),
            new ::CheckboxPropertyComponent("collapsePane2", panel->getIsPane2Collapsed(),
                [panel](bool shouldCollapse)
                {
                    panel->setPane2Collapsed(shouldCollapse);
                })
        });
        
        propertyPanel.addSection("Actions", {
            new ::SplitPanelSeparatorPosPropertyComponent<int>("Separator pos", panel->getSeparatorPos(), 0,
                                                               std::numeric_limits<int>::max(), *panel,
                [panel](int value)
                {
                    panel->setSeparatorPos(value);
                },
                [panel]()
                {
                    return panel->getSeparatorPos();
                }),
            new ::SplitPanelSeparatorPosPropertyComponent<double>("Separator pos (FS)",
                                                                  panel->getSeparatorPosNormalised(), 0.0, 1.0, *panel,
                [panel](double value)
                {
                    panel->setSeparatorPosNormalised(value);
                },
                [panel]()
                {
                    return panel->getSeparatorPosNormalised();
                }),
            new ::NumberActionPropertyComponent<int>("Move separator", 0, std::numeric_limits<int>::min(),
                                                                          std::numeric_limits<int>::max(),
                [panel](int value)
                {
                    panel->moveSeparator(value);
                }),
            new ::NumberActionPropertyComponent<double>("Move separator (FS)", 0, -1.0, 1.0,
                [panel](double value)
                {
                    panel->moveSeparatorNormalised(value);
                })
        });
    }
    
    propertyPanel.addSection("Style", style_properties, true, 0);
    
    resized();
}
//======================================================================================================================
// endregion GuiExamples
//**********************************************************************************************************************
