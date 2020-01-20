#include "RecordChannelSelector.h"

ChannelButton::ChannelButton(int _id) : Button(String(_id)), id(_id) {
    setClickingTogglesState(true);
}
ChannelButton::~ChannelButton() {}

void ChannelButton::paintButton(Graphics &g, bool isMouseOver, bool isButtonDown)
{

	g.setColour(Colour(0,0,0));
	g.fillRoundedRectangle (0.0f, 0.0f, getWidth(), getHeight(), 0.1*getWidth());

	if (isMouseOver)
	{
		if (getToggleState())
			g.setColour(Colour(255, 65, 65));
		else
			g.setColour(Colour(210, 210, 210));
	}
	else 
	{
		if (getToggleState())
			g.setColour(Colour(255, 0, 0));
		else
			g.setColour(Colour(110, 110, 110));
	}
	g.fillEllipse(1,1,getWidth()-2,getHeight()-2);

    //Draw text string in middle of button
	g.setColour(Colour(255,255,255));
	g.setFont(10);
	g.drawText (String(id), 0,0, getWidth(), getHeight(), Justification::centred); 

}

SelectButton::SelectButton(const String& name) : Button(name) {
	setClickingTogglesState(true);
}

SelectButton::~SelectButton() {}

void SelectButton::paintButton(Graphics &g, bool isMouseOver, bool isButtonDown)
{
    g.setColour(Colour(0,0,0));
    g.fillRoundedRectangle (0.0f, 0.0f, getWidth(), getHeight(), 0.001*getWidth());

    if (isMouseOver)
    {
        g.setColour(Colour(220, 220, 220));
    }
    else
    {
        if (getToggleState())
            g.setColour(Colour(200, 200, 200));
        else
            g.setColour(Colour(110, 110, 110));
    }
    g.fillRoundedRectangle(0.0f, 0.0f, getWidth(), getHeight(), 0.01*getWidth());
    
	g.setColour(Colour(255,255,255));
	g.setFont(12);
	g.drawText (String(getName()), 0, 0, getWidth(), getHeight(), Justification::centred);
}

RangeEditor::RangeEditor(const String& name) : TextEditor(name)
{

}

RangeEditor::~RangeEditor() {}

/**************************
 * RECORD CHANNEL SELECTOR
***************************/

RecordChannelSelector::RecordChannelSelector(int nChannels) : Component()
{
    //Creates the grid based on the number of channel

    //Max width = 20*24;

    //If <= 64, generate one rows of 8
    //If <= 256, generate rows of 16
    //If <= 512, generate rows of 24

    int nColumns = 16;
    int nRows = nChannels / nColumns + (int)(!(nChannels % nColumns == 0));
    int width = 480;
    int buttonSize = width / 16;
    int height = buttonSize * nRows;

	for (int i = 0; i < nRows; i++)
	{
		for (int j = 0; j < nColumns; j++)
		{
            if (nColumns*i+j < nChannels)
            {
                channelButtons.add(new ChannelButton(nColumns*i+j+1));
                channelButtons.getLast()->setBounds(width/nColumns*j, height/nRows*i, buttonSize, buttonSize);
                //channelButtons.getLast()->addListener(this);
                addChildAndSetID(channelButtons.getLast(), String(nColumns*i+j+1));
            }
			
		}
	}

    //Add "SELECT ALL" button
	selectButtons.add(new SelectButton("ALL"));
    selectButtons.getLast()->setBounds(0, height, 0.25*width, width / nColumns);
	addChildAndSetID(selectButtons.getLast(),"ALL");

    //Add "SELECT NONE" button
	selectButtons.add(new SelectButton("NONE"));
    selectButtons.getLast()->setBounds(0.25*width, height, 0.25*width, width / nColumns);
	addChildAndSetID(selectButtons.getLast(),"ALL");
    
    if (nChannels > 8)
    {
        //Add "SELECT RANGE" button
        selectButtons.add(new SelectButton("RANGE:"));
        selectButtons.getLast()->setBounds(0.5*width, height, 0.25*width, width / nColumns);
        addChildAndSetID(selectButtons.getLast(),"ALL");

        //Add Range Edtior
        
        rangeEditor = new RangeEditor("Range");
        rangeEditor->setBounds(0.75*width, height, 0.25*width, width / nColumns);
        addChildAndSetID(rangeEditor,"RANGE_EDITOR");
        
    }
    
    if (nChannels <= 8)
        width /= 2;

	setSize (width, height + buttonSize);
	setColour(ColourSelector::backgroundColourId, Colours::transparentBlack);

}

RecordChannelSelector::~RecordChannelSelector() {}
