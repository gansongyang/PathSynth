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
ControlPointComponent::ControlPointComponent(AudioProcessorValueTreeState& parameters, const int index, int radius) :
    parameters(parameters), index(index), radius(radius)
{
    constrainer.setMinimumOnscreenAmounts(radius, radius,
                                          radius, radius);
}

ControlPointComponent::~ControlPointComponent()
{
}

void ControlPointComponent::paint(Graphics& g)
{
    auto rect = getLocalBounds().toFloat();
    g.setColour(Colour(0xffa84350));
    g.drawEllipse(rect, 1.0f);

    g.setColour(Colour(0xffdac9cb));
    rect.reduce(2.0f, 2.0f);
    g.drawEllipse(rect, 1.0f);

    g.setColour(Colour(0xff97b0c4));
    rect.reduce(2.0f, 2.0f);
    g.drawEllipse(rect, 1.0f);

    g.setColour(Colour(0xffdcc296));
    rect.reduce(2.0f, 2.0f);
    g.drawEllipse(rect, 1.0f);

    g.setColour(Colour(0xffa84350));
    rect.reduce(2.0f, 2.0f);
    g.drawEllipse(rect, 1.0f);

    if (index == 0)
    {
        g.setColour(Colour(0xaaffffff));
        g.fillEllipse(rect);
    }
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
    dragger.dragComponent(this, event, &constrainer);

    const auto parentComponent = getParentComponent();
    const auto newPosition = getBounds().getPosition();

    auto newPositionX = static_cast<float>(newPosition.getX());
    newPositionX += getBounds().getWidth() * 0.5f;
    newPositionX /= static_cast<float>(parentComponent->getWidth());

    auto newPositionY = static_cast<float>(newPosition.getY());
    newPositionY += getBounds().getHeight() * 0.5f;
    newPositionY /= static_cast<float>(parentComponent->getHeight());

    if (auto* p = parameters.getParameter("point" + String(index) + "x"))
    {
        if (p->getValue() != newPositionX)
            p->setValueNotifyingHost(newPositionX);
    }
    if (auto* p = parameters.getParameter("point" + String(index) + "y"))
    {
        if (p->getValue() != newPositionY)
            p->setValueNotifyingHost(newPositionY);
    }
}
