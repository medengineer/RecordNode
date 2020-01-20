#include "RecordChannelSelector.h"
#include <string>
#include <vector>

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

RecordChannelSelector::RecordChannelSelector(int nChannels) : Component(), nChannels(nChannels)
{

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
                channelButtons.getLast()->addListener(this);
                addChildAndSetID(channelButtons.getLast(), String(nColumns*i+j+1));
            }
			
		}
	}

    //Add "SELECT ALL" button
	selectButtons.add(new SelectButton("ALL"));
    selectButtons.getLast()->setBounds(0, height, 0.25*width, width / nColumns);
    selectButtons.getLast()->addListener(this);
	addChildAndSetID(selectButtons.getLast(),"ALL");

    //Add "SELECT NONE" button
	selectButtons.add(new SelectButton("NONE"));
    selectButtons.getLast()->setBounds(0.25*width, height, 0.25*width, width / nColumns);
    selectButtons.getLast()->addListener(this);
	addChildAndSetID(selectButtons.getLast(),"ALL");
    
    if (nChannels > 8)
    {

        //Add "SELECT RANGE" button
        selectButtons.add(new SelectButton("RANGE"));
        selectButtons.getLast()->setBounds(0.5*width, height, 0.25*width, width / nColumns);
        selectButtons.getLast()->addListener(this);
        addChildAndSetID(selectButtons.getLast(),"ALL");

        //Add Range Editor
        
        rangeEditor = new RangeEditor("Range");
        rangeEditor->setBounds(0.75*width, height, 0.25*width, width / nColumns);
        rangeEditor->addListener(this);
        addChildAndSetID(rangeEditor,"RANGE_EDITOR");
        
    }
    
    if (nChannels <= 8)
        width /= 2;

	setSize (width, height + buttonSize);
	setColour(ColourSelector::backgroundColourId, Colours::transparentBlack);

}

RecordChannelSelector::~RecordChannelSelector() {}

void RecordChannelSelector::textEditorReturnKeyPressed(TextEditor& editor)
{
    channelStates = parseStringIntoRange(9);

    if (channelStates.size() < 3)
        return;

    for (int i = 0; i < nChannels; i++)
        channelButtons[i++]->setToggleState(false, NotificationType::dontSendNotification);

    int i = 0;
    while (i <= channelStates.size() - 3)
    {
        const int lim = channelStates[i+1];
        const int comd = channelStates[i+2];
        for (int fa = channelStates[i]; fa < lim; fa += comd)
        {
            channelButtons[i++]->setToggleState(true, NotificationType::dontSendNotification);
        }
        i+=3;
    }

    /*
    parseStringIntoRange returns an array of size N where N/3 is the number of ranges to set.
    Each range "tuple" is represented by (startIdx, endIndex, step)
    */

    for (auto val : channelStates)
    {
        std::cout << val << ",";
    }
    std::cout << std::endl;

    i = 0;
    while (i < nChannels)
    {
        channelButtons[i++]->setToggleState(false, NotificationType::dontSendNotification);
    }

    for (i = 0; i < channelStates.size(); i += 3)
    {
        int startIdx = channelStates[i];
        int endIdx = channelStates[i+1];
        int step = channelStates[i+2];
        
        int ch = startIdx;
        while (ch < endIdx)
        {
            channelButtons[ch]->setToggleState(true, NotificationType::dontSendNotification);
            ch+=step;
        }

    }
}

void RecordChannelSelector::buttonClicked(Button* button)
{
    
    for (auto* button : selectButtons)
    {
        button->setToggleState(false, NotificationType::dontSendNotification);
    }
    
    if (button->getButtonText() == String("ALL"))
    {
        std::cout << "Select all channels" << std::endl;
        int i = 0;
        while (i < nChannels)
        {
            channelButtons[i++]->setToggleState(true, NotificationType::dontSendNotification);
        }
        rangeString = "[" + String(1) + ":" + String(nChannels) + "]";
        
        button->setToggleState(true, NotificationType::dontSendNotification);
        
    }
    else if (button->getButtonText() == String("NONE"))
    {
        std::cout << "Select no channels" << std::endl;
        int i = 0;
        while (i < nChannels)
        {
            channelButtons[i++]->setToggleState(false, NotificationType::dontSendNotification);
        }
        rangeString = "[]";
        
        button->setToggleState(true, NotificationType::dontSendNotification);
    }
    else if (button->getButtonText() == String("RANGE:"))
    {
        std::cout << "Set custom range of channels" << std::endl;
        button->setToggleState(true, NotificationType::dontSendNotification);
    }
    else //channel button was manually selected
    {
        selectButtons.getLast()->setToggleState(true, NotificationType::dontSendNotification);
        int channel = ((ChannelButton*)button)->getId();
        updateRangeString();
    }
    
    rangeEditor->setText(rangeString);
    
}

void RecordChannelSelector::updateRangeString()
{
    
    rangeString = "";
    
    int startIdx = 0;
    int endIdx = 0;

    bool inRange = false;
    
    for (int i = 0; i < nChannels; i++)
    {
        if (channelButtons[i]->getToggleState())
        {
            if (inRange)
            {
                if (i == nChannels-1)
                {
                    rangeString += (rangeString == "" ? "" : ",");
                    rangeString += String(startIdx) + ":" + String(nChannels);
                }
            }
            else
            {
                if (i == nChannels-1)
                {
                    rangeString += (rangeString == "" ? "" : ",");
                    rangeString += String(nChannels);
                }
                startIdx = i + 1;
            }
            inRange = true;
            endIdx = i + 1;
        }
        else
        {
            if (inRange)
            {
                rangeString += (rangeString == "" ? "" : ",");
                if (startIdx < endIdx)
                    rangeString += String(startIdx) + ":" + String(endIdx);
                else
                    rangeString += String(startIdx);
            }
            else
            {
                
            }
            inRange = false;
        }
        
    }
    
}

int RecordChannelSelector::convertStringToInteger(String s)
{
    char ar[20];
    int i, k = 0;
    for (i = 0; i < s.length(); i++)
    {
        if (s[i] >= 48 && s[i] <= 57)
        {
            ar[k] = s[i];
            k++;
        }
    }
    if (k > 7)
    {
        return 1000000;
    }
    ar[k] = '\0';
    k = atoi(ar);
    return k;
}

Array<int> RecordChannelSelector::parseStringIntoRange(int rangeValue)
{
    String s = ",";
    s += rangeEditor->getText();
    Array<int> finalList, separator, rangeseparator;
    int i, j, a, b, k, openb, closeb, otherchar, x, y;
    s += ",";
    for (i = 0; i < s.length(); i++) //split string by ' , ' or ' ; '
    {
        if (s[i] == ';' || s[i] == ',')
        {
            separator.add(i);
        }
    }
    for (i = 0; i < separator.size() - 1; i++) // split ranges by ' : ' or ' - '
    {
        j = k = separator[i] + 1;
        openb = closeb = otherchar = 0;
        rangeseparator.clear();
        for (; j < separator[i + 1]; j++)
        {
            if (s[j] == '-' || s[j] == ':')
            {
                rangeseparator.add(j);
            }
            else if (((int)s[j] == 32))
            {
                continue;
            }
            else if (s[j] == '[' || s[j] == '{' || s[j] == '(')
            {
                openb++;
            }
            else if (s[j] == ']' || s[j] == '}' || s[j] == ')')
            {
                closeb++;
            }
            else if ((int)s[j] > 57 || (int)s[j] < 48)
            {
                otherchar++;
            }
        }

        if (openb != closeb || openb > 1 || closeb > 1 || otherchar > 0) //Invalid input
        {
            continue;
        }

        for (x = separator[i] + 1; x < separator[i + 1]; x++) //trim whitespace and brackets from front
        {
            if (((int)s[x] >= 48 && (int)s[x] <= 57) || s[x] == ':' || s[x] == '-')
            {
                break;
            }
        }
        for (y = separator[i + 1] - 1; y > separator[i]; y--) //trim whitespace and brackets from end
        {
            if (((int)s[y] >= 48 && (int)s[y] <= 57) || s[y] == ':' || s[y] == '-')
            {
                break;
            }
        }
        if (x > y)
        {
            continue;
        }

        if (rangeseparator.size() == 0) //syntax of form - x or [x]
        {
            a = convertStringToInteger(s.substring(x, y + 1));
            if (a == 0 || a > rangeValue)
            {
                continue;
            }
            finalList.add(a - 1);
            finalList.add(a - 1);
            finalList.add(1);
        }
        else if (rangeseparator.size() == 1) // syntax of type - x-y or [x-y]
        {
            a = convertStringToInteger(s.substring(x, rangeseparator[0]));
            b = convertStringToInteger(s.substring(rangeseparator[0] + 1, y + 1));
            if (a == 0)
            {
                a = 1;
            }
            if (b == 0)
            {
                b = rangeValue;
            }
            if (a > b || a > rangeValue || b > rangeValue)
            {
                continue;
            }
            finalList.add(a - 1);
            finalList.add(b - 1);
            finalList.add(1);
        }
        else if (rangeseparator.size() == 2) // syntax of type [x:y:z] or x-y-z
        {
            a = convertStringToInteger(s.substring(x, rangeseparator[0] + 1));
            k = convertStringToInteger(s.substring(rangeseparator[0] + 1, rangeseparator[1]));
            b = convertStringToInteger(s.substring(rangeseparator[1] + 1, y + 1));

            if (a == 0)
            {
                a = 1;
            }
            if (b == 0)
            {
                b = rangeValue;
            }
            if (k == 0)
            {
                k = 1;
            }
            if (a > b || a > rangeValue || b > rangeValue)
            {
                continue;
            }
            finalList.add(a - 1);
            finalList.add(b - 1);
            finalList.add(k);
        }
    }
    return finalList;
}
