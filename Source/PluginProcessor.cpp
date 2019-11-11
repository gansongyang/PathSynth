#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "PathSynthConstants.h"
#include "PathVoice.h"
#include "PathSound.h"

AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
{
    std::vector<std::unique_ptr<RangedAudioParameter>> params;

    params.push_back(std::make_unique<AudioParameterFloat>("smoothing",
                                                           "Smoothing",
                                                           NormalisableRange<float>(0.0f,
                                                                                    1.0f,
                                                                                    0.0f,
                                                                                    0.9f,
                                                                                    false),
                                                           100.0f));
    params.push_back(std::make_unique<AudioParameterChoice>("direction",
                                                            "Direction",
                                                            StringArray{"X", "Y"},
                                                            0));
    for (auto i = 0; i < PathSynthConstants::numControlPoints; ++i)
    {
        auto x = std::cos((static_cast<float>(i) / PathSynthConstants::numControlPoints)
            * MathConstants<float>::twoPi) * .25f;
        auto y = std::sin((static_cast<float>(i) / PathSynthConstants::numControlPoints)
            * MathConstants<float>::twoPi) * .25f;

        params.push_back(std::make_unique<AudioParameterFloat>("point" + String(i) + "x",
                                                               "Point" + String(i) + "_X",
                                                               NormalisableRange<float>(-1.0f,
                                                                                        1.0f,
                                                                                        0.0f,
                                                                                        1.0f,
                                                                                        false),
                                                               x));

        params.push_back(std::make_unique<AudioParameterFloat>("point" + String(i) + "y",
                                                               "Point" + String(i) + "_Y",
                                                               NormalisableRange<float>(-1.0f,
                                                                                        1.0f,
                                                                                        0.0f,
                                                                                        1.0f,
                                                                                        false),
                                                               y));
    }
    params.push_back(std::make_unique<AudioParameterFloat>("attack",
                                                           "Attack",
                                                           NormalisableRange<float>(0.0f,
                                                                                    20000.0f,
                                                                                    0.0f,
                                                                                    0.2f,
                                                                                    false),
                                                           1.0f,
                                                           String(),
                                                           AudioProcessorParameter::genericParameter,
                                                           [](const float value, int /*maximumStringLength*/)
                                                           {
                                                               if (value >= 1000.0f)
                                                                   return String(value / 1000.0f, 2) + " s";
                                                               return String(value, 2) + " ms";
                                                           },
                                                           [](const String& text)
                                                           {
                                                               if (text.endsWith(" s") ||
                                                                   (text.endsWith("s") && !text.endsWith("ms")))
                                                               {
                                                                   return text.getFloatValue() * 1000.0f;
                                                               }
                                                               return text.getFloatValue();
                                                           }));
    params.push_back(std::make_unique<AudioParameterFloat>("decay",
                                                           "Decay",
                                                           NormalisableRange<float>(1.0f,
                                                                                    60000.0f,
                                                                                    0.0f,
                                                                                    0.2f,
                                                                                    false),
                                                           600.0f,
                                                           String(),
                                                           AudioProcessorParameter::genericParameter,
                                                           [](const float value, int /*maximumStringLength*/)
                                                           {
                                                               if (value >= 1000.0f)
                                                                   return String(value / 1000.0f, 2) + " s";
                                                               return String(value, 2) + " ms";
                                                           },
                                                           [](const String& text)
                                                           {
                                                               if (text.endsWith(" s") ||
                                                                   (text.endsWith("s") && !text.endsWith("ms")))
                                                               {
                                                                   return text.getFloatValue() * 1000.0f;
                                                               }
                                                               return text.getFloatValue();
                                                           }));
    params.push_back(std::make_unique<AudioParameterFloat>("sustain",
                                                           "Sustain",
                                                           NormalisableRange<float>(0.0f,
                                                                                    1.0,
                                                                                    0.0f,
                                                                                    1.0f,
                                                                                    false),
                                                           1.0f,
                                                           String(),
                                                           AudioProcessorParameter::genericParameter,
                                                           [](const float value, int /*maximumStringLength*/)
                                                           {
                                                               return String(Decibels::gainToDecibels(value), 2) +
                                                                   " dB";
                                                           },
                                                           [](const String& text)
                                                           {
                                                               return Decibels::decibelsToGain(text.getFloatValue());
                                                           }));
    params.push_back(std::make_unique<AudioParameterFloat>("release",
                                                           "Release",
                                                           NormalisableRange<float>(1.0f,
                                                                                    60000.0f,
                                                                                    0.0f,
                                                                                    0.2f,
                                                                                    false),
                                                           50.0f,
                                                           String(),
                                                           AudioProcessorParameter::genericParameter,
                                                           [](const float value, int /*maximumStringLength*/)
                                                           {
                                                               if (value >= 1000.0f)
                                                                   return String(value / 1000.0f, 2) + " s";
                                                               return String(value, 2) + " ms";
                                                           },
                                                           [](const String& text)
                                                           {
                                                               if (text.endsWith(" s") ||
                                                                   (text.endsWith("s") && !text.endsWith("ms")))
                                                               {
                                                                   return text.getFloatValue() * 1000.0f;
                                                               }
                                                               return text.getFloatValue();
                                                           }));
    return {params.begin(), params.end()};
}

//==============================================================================
PathSynthAudioProcessor::PathSynthAudioProcessor(): AudioProcessor(
                                                        BusesProperties().withOutput("Output",
                                                                                     AudioChannelSet::stereo(),
                                                                                     true)),
                                                    parameters(*this,
                                                               nullptr,
                                                               "PathSynth",
                                                               createParameterLayout())
{
    for (auto i = 0; i < numVoices; ++i)
    {
        synthesiser.addVoice(new PathVoice(parameters, processorPath, envParams));
    }
    synthesiser.addSound(new PathSound());
}

PathSynthAudioProcessor::~PathSynthAudioProcessor()
{
}

//==============================================================================
const String PathSynthAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PathSynthAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool PathSynthAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool PathSynthAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double PathSynthAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PathSynthAudioProcessor::getNumPrograms()
{
    return 1; // NB: some hosts don't cope very well if you tell them there are 0 programs,
    // so this should be at least 1, even if you're not really implementing programs.
}

int PathSynthAudioProcessor::getCurrentProgram()
{
    return 0;
}

void PathSynthAudioProcessor::setCurrentProgram(int index)
{
}

const String PathSynthAudioProcessor::getProgramName(int index)
{
    return {};
}

void PathSynthAudioProcessor::changeProgramName(int index, const String& newName)
{
}

//==============================================================================
void PathSynthAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    synthesiser.setCurrentPlaybackSampleRate(sampleRate); // todo * oversampleFactor
}

void PathSynthAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.    
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool PathSynthAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
#if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}
#endif

void PathSynthAudioProcessor::processBlock(AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    const auto totalNumOutputChannels = getTotalNumOutputChannels();

    buffer.clear(0, 0, buffer.getNumSamples());

    setPath();

    envParams.attack = *parameters.getRawParameterValue("attack") / 1000.0f;
    envParams.decay = *parameters.getRawParameterValue("decay") / 1000.0f;
    envParams.sustain = *parameters.getRawParameterValue("sustain");
    envParams.release = *parameters.getRawParameterValue("release") / 1000.0f;

    keyboardState.processNextMidiBuffer(midiMessages, 0,
                                        buffer.getNumSamples(), true);
    synthesiser.renderNextBlock(buffer, midiMessages,
                                0, buffer.getNumSamples());

    // copy the processed channel to all the other channels
    for (auto i = 1; i < totalNumOutputChannels; ++i)
        buffer.copyFrom(i, 0, buffer, 0, 0, buffer.getNumSamples());

    midiMessages.clear();
}

//==============================================================================
bool PathSynthAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* PathSynthAudioProcessor::createEditor()
{
    return new PathSynthAudioProcessorEditor(*this, parameters, keyboardState);
}

//==============================================================================
void PathSynthAudioProcessor::getStateInformation(MemoryBlock& destData)
{
    const auto state = parameters.copyState();
    const auto xml(state.createXml());
    xml->setAttribute("maxVoices", numVoices);
    copyXmlToBinary(*xml, destData);
}

void PathSynthAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    const auto xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState != nullptr)
    {
        if (xmlState->hasAttribute("maxVoices"))
        {
            setNumVoices(xmlState->getIntAttribute("maxVoices", 10));
            xmlState->removeAttribute("maxVoices");
        }
        if (xmlState->hasTagName(parameters.state.getType()))
        {
            parameters.replaceState(ValueTree::fromXml(*xmlState));
        }
    }
}

void PathSynthAudioProcessor::setNumVoices(int newNumVoices)
{
    numVoices = newNumVoices;
    const auto synthNumVoices = synthesiser.getNumVoices();
    if (synthNumVoices == numVoices)
    {
        return;
    }
    if (synthNumVoices < numVoices)
    {
        for (auto i = synthNumVoices; i < numVoices; ++i)
        {
            synthesiser.addVoice(new PathVoice(parameters, processorPath, envParams));
        }
        jassert(numVoices==synthesiser.getNumVoices());
        return;
    }
    // synthNumVoices > numVoices
    for (auto i = synthNumVoices; i > numVoices; --i)
    {
        synthesiser.removeVoice(i - 1);
    }
    jassert(numVoices==synthesiser.getNumVoices());
}

int PathSynthAudioProcessor::getNumVoices()
{
    jassert(numVoices==synthesiser.getNumVoices());
    return numVoices;
}

//==============================================================================
void PathSynthAudioProcessor::setPath()
{
    //todo check if it changed
    straightPath.clear();

    const Point<float> firstPointPos{
        *parameters.getRawParameterValue("point0x"),
        *parameters.getRawParameterValue("point0y")
    };
    straightPath.startNewSubPath(firstPointPos);

    for (auto i = 1; i < PathSynthConstants::numControlPoints; ++i)
    {
        const Point<float> pointPos{
            *parameters.getRawParameterValue("point" + String(i) + "x"),
            *parameters.getRawParameterValue("point" + String(i) + "y")
        };
        straightPath.lineTo(pointPos);
    }
    straightPath.closeSubPath();

    const auto smoothing = *parameters.getRawParameterValue("smoothing");
    processorPath = straightPath.createPathWithRoundedCorners(smoothing);

    const auto smoothPathBounds = processorPath.getBounds();
    processorPath.applyTransform(
        AffineTransform::translation(
            -smoothPathBounds.getCentreX(),
            -smoothPathBounds.getCentreY()).followedBy(AffineTransform::scale(1.0f / smoothPathBounds.getWidth(),
                                                                              1.0f / smoothPathBounds.getHeight())));
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PathSynthAudioProcessor();
}
