/*
------------------------------------------------------------------

This file is part of the Open Ephys GUI
Copyright (C) 2014 Open Ephys

------------------------------------------------------------------

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include <stdio.h>
#include "RecordNodeEditor.h"
#include "RecordNode.h"

RecordNodeEditor::RecordNodeEditor(RecordNode* parentNode, bool useDefaultParameterEditors = true)
	: GenericEditor(parentNode, useDefaultParameterEditors)

{

	recordNode = parentNode;

	desiredWidth = 120;

	scaleStringLabel = new Label("scaleLabel", "Copy time:");
	scaleStringLabel->setBounds(10, 23, 80, 20);
	scaleStringLabel->setFont(Font("Small Text", 11, Font::plain));
	scaleStringLabel->setColour(Label::textColourId, Colours::darkgrey);
	addAndMakeVisible(scaleStringLabel);

	convertStringLabel = new Label("convertLabel", "Convert time:");
	convertStringLabel->setBounds(10, 58, 80, 20);
	convertStringLabel->setFont(Font("Small Text", 11, Font::plain));
	convertStringLabel->setColour(Label::textColourId, Colours::darkgrey);
	addAndMakeVisible(convertStringLabel);

	writeStringLabel = new Label("writeLabel", "Write time:");
	writeStringLabel->setBounds(10, 93, 80, 20);
	writeStringLabel->setFont(Font("Small Text", 11, Font::plain));
	writeStringLabel->setColour(Label::textColourId, Colours::darkgrey);
	addAndMakeVisible(writeStringLabel);

	scaleTimeLabel = new Label("scaleTime", "0");
	scaleTimeLabel->setBounds(15, 40, 60, 18);
	scaleTimeLabel->setFont(Font("Default", 14, Font::plain));
	scaleTimeLabel->setColour(Label::textColourId, Colours::white);
	scaleTimeLabel->setColour(Label::backgroundColourId, Colours::grey);
	addAndMakeVisible(scaleTimeLabel);

	convertTimeLabel = new Label("convertTime", "0");
	convertTimeLabel->setBounds(15, 75, 60, 18);
	convertTimeLabel->setFont(Font("Default", 14, Font::plain));
	convertTimeLabel->setColour(Label::textColourId, Colours::white);
	convertTimeLabel->setColour(Label::backgroundColourId, Colours::grey);
	addAndMakeVisible(convertTimeLabel);

	writeTimeLabel = new Label("writeTime", "0");
	writeTimeLabel->setBounds(15, 110, 60, 18);
	writeTimeLabel->setFont(Font("Default", 14, Font::plain));
	writeTimeLabel->setColour(Label::textColourId, Colours::white);
	writeTimeLabel->setColour(Label::backgroundColourId, Colours::grey);
	addAndMakeVisible(writeTimeLabel);

	startTimer(500);

}

RecordNodeEditor::~RecordNodeEditor()
{

}

void RecordNodeEditor::timerCallback()
{

	int scaleTime = recordNode->scaleTime;
	int convertTime = recordNode->convertTime;
	int writeTime = recordNode->writeTime;

	float total = (scaleTime + convertTime + writeTime) / 100;

	if (total > 0)
	{
		scaleTimeLabel->setText(String(scaleTime / total), dontSendNotification);
		convertTimeLabel->setText(String(convertTime / total), dontSendNotification);
		writeTimeLabel->setText(String(writeTime / total), dontSendNotification);
	}

}


