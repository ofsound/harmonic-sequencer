#include "PluginEditor.h"

//==============================================================================
PluginEditor::PluginEditor (PluginProcessor& p)
    : AudioProcessorEditor (&p),
      processorRef (p)
{
#if JUCE_WEB_BROWSER
    webView = std::make_unique<juce::WebBrowserComponent> (WebViewResources::makeBrowserOptions (processorRef));
    addAndMakeVisible (*webView);

   #if JUCE_DEBUG && defined (HARMONIC_SEQUENCER_UI_DEV_SERVER)
    webView->goToURL ("http://localhost:5175");
   #else
    webView->goToURL (juce::WebBrowserComponent::getResourceProviderRoot());
   #endif
#else
    fallbackLabel.setText ("WebView is not available on this platform.\nEnable JUCE_WEB_BROWSER.",
                           juce::dontSendNotification);
    fallbackLabel.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (fallbackLabel);
#endif

    setResizeLimits (800, 480, 2000, 1200);
    setResizable (true, true);
    setSize (1200, 720);
}

PluginEditor::~PluginEditor() = default;

void PluginEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff1a1a1a));
}

void PluginEditor::resized()
{
#if JUCE_WEB_BROWSER
    if (webView != nullptr)
        webView->setBounds (getLocalBounds());
#else
    fallbackLabel.setBounds (getLocalBounds());
#endif
}
