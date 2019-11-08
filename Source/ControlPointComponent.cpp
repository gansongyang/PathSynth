/*
  ==============================================================================

    ControlPointComponent.cpp
    Created: 4 Nov 2019 10:53:48am
    Author:  Luke

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "ControlPointComponent.h"

//==============================================================================
ControlPointComponent::
ControlPointComponent(AudioProcessorValueTreeState& parameters, int index) :
    parameters(parameters), index(index)
{
}

ControlPointComponent::~ControlPointComponent()
{
}

void ControlPointComponent::paint(Graphics& g)
{
    g.fillAll(Colours::white);
}

void ControlPointComponent::resized()
{
}

void ControlPointComponent::mouseDown(const MouseEvent& event)
{
    dragger.startDraggingComponent(this, event);
}

void ControlPointComponent::mouseDrag(const MouseEvent& event)
{
    dragger.dragComponent(this, event, nullptr);

    const auto parentComponent = getParentComponent();
    const auto newPosition = event.getEventRelativeTo(parentComponent).getPosition();
    const auto newPositionX = 2.0f * (static_cast<float>(newPosition.getX()) / parentComponent->getWidth()) - 1.0f;
    const auto newPositionY = 2.0f * (static_cast<float>(newPosition.getY()) / parentComponent->getHeight()) - 1.0f;
    DBG(newPositionX);
    if (auto* p = parameters.getParameter("point" + String(index) + "x"))
    {
        //todo reuse p for this?
        const float newValue = parameters.getParameterRange("point" + String(index) + "x").convertTo0to1(newPositionX);

        if (p->getValue() != newValue)
            p->setValueNotifyingHost(newValue);
    }
    if (auto* p = parameters.getParameter("point" + String(index) + "y"))
    {
        //todo reuse p for this?
        const float newValue = parameters.getParameterRange("point" + String(index) + "y").convertTo0to1(newPositionY);

        if (p->getValue() != newValue)
            p->setValueNotifyingHost(newValue);
    }
}
